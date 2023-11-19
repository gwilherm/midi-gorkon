#ifndef MIDI_ADDRESSABLE_H
#define MIDI_ADDRESSABLE_H

#include <Control_Surface.h>

struct MIDIAddressable {
  virtual ~MIDIAddressable() = default;
  virtual MIDIAddress getAddress() const = 0;
  virtual void setAddressUnsafe(MIDIAddress address) = 0;
};

template <class T, class... Args>
MIDIAddressable *make_addressable(Args &&... args) {
  struct MIDIAddressableT : MIDIAddressable, T {
    MIDIAddressableT(Args &&... args) : T(std::forward<Args>(args)...) {}
    MIDIAddress getAddress() const override { return T::getAddress(); }
    void setAddressUnsafe(MIDIAddress address) override { T::setAddressUnsafe(address); }
  };
  return new MIDIAddressableT(std::forward<Args>(args)...);
}

#endif // MIDI_ADDRESSABLE_H