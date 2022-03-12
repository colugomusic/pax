# pax

It's a c++ PortAudio wrapper. I use this in Blockhead.

In Blockhead I require:
 - A single audio stream to be open at a time with 2 output channels and any number of input channels.
 - Ability to hot-switch between audio devices
 - Ability to query if a device is a WASAPI loopback device
 - Ability to rescan available audio devices

If this matches your requirements then maybe this is useful for you.

## pax::System

Queries all the available audio hosts and devices and provides an interface for more easily dealing with them.

It is also an RAII wrapper around `Pa_Initialize()` and `Pa_Terminate()` so you should only keep one alive at once.

To rescan audio devices, just destroy your current `pax::System` and create a new one.

This is more or less how I do things in Blockhead:

```c++
class AudioSystem
{
public:

  ...
  
  auto rescan_devices() -> void
  {
    // Stop the stream immediately without waiting for it to finish cleanly
    stream_.abort();

    // Make sure the existing object is destructed before creating a new one
    system_.reset();
    system_ = std::make_unique<pax::System>();
    
    // I generate a pax::Stream::Request from user settings, or fall back to defaults
    request_stream(create_stream_request_from_settings());
    
    // Blockhead specific crap
    notify_(N::devices_rescanned);
  }
  
  ...
  
  //
  // A wrapper around pax::Stream::request() which checks if the stream is already active.
  //
  // If it is, the request is put on a queue so it happens once the current stream
  // finishes instead. Then pax::Stream::stop() is called.
  //
  // A "request" in this context is "a request for an audio stream to be started with
  // the given stream settings, which may or may not actually succeed"
  //
  // If the request fails for some reason then pax::Stream::Config.callbacks.error is called
  //
  auto request_stream(pax::Stream::Request settings) -> void
  {
    // If stream is running, close it and queue this request
    if (stream_.is_active())
    {
      // pax::Stream::push_finished_task() can be used to push tasks to be run once
      // the stream finishes
      stream_.push_finished_task([this, settings]()
      {
        // This task will be run in some other thread so I do a mixture of
        // Godot/boost::signals2 nonsense here to defer the operation until the
        // next GUI frame
        Deferred::i().parallel().push([this, settings]()
        {
          request_stream(settings);
        });
      });

      // The task we just pushed onto the queue will be performed once the stream stops.
      // Calling stop() here doesn't stop the stream immediately, it will try to
      // finish cleanly
      stream_.stop();
      return;
    }

    stream_.request(settings);
  }
  
  ...
  
private:

  ...
  
  std::unique_ptr<pax::System> system_;
  pax::Stream stream_;
  
  ...
  
};
```

## pax::Stream

A stream can be active or inactive and can be switched between different configurations via `pax::Stream::request()`.

`request()` does nothing if the stream is already active.
