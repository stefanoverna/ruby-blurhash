require "blurhash/version"
require "ffi"

module Blurhash
  def self.decode(blurhash, width, height, punch: 1.0)
    out_pointer = FFI::MemoryPointer.new(:pointer)

    out_size_t = Unstable.blurhash_decode(
      blurhash,
      width,
      height,
      punch,
      out_pointer,
    )

    out_result = out_pointer.get_pointer(0)

    result = out_result.read_array_of_uint8(out_size_t)

    Unstable.blurhash_free(out_result)

    result
  end

  module Unstable
    extend FFI::Library
    ffi_lib File.join(File.expand_path(__dir__), 'blurhash.' + RbConfig::CONFIG['DLEXT'])

    attach_function :blurhash_decode, %i(string int int float pointer), :size_t
    attach_function :blurhash_free, %i(pointer), :void
  end

  private_constant :Unstable
end
