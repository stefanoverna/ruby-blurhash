require "blurhash/version"
require "ffi"
require "base64"

module Blurhash
  def self.compress(pixels, width, height, quality: 70)
    in_pointer = FFI::MemoryPointer.new(:uint8, pixels.size)
    in_pointer.write_array_of_uint8(pixels)
    out_pointer = FFI::MemoryPointer.new(:pointer)
    out_size_t = Unstable.BlurhashCompress(
      in_pointer,
      width,
      height,
      quality,
      out_pointer,
    )
    out_result = out_pointer.get_pointer(0)
    result = out_result.read_array_of_uint8(out_size_t)
    Unstable.BlurhashFreeBuffer(out_result)

    "data:image/jpeg;base64," + Base64.encode64(result.pack('C*')).split("\n").join
  ensure
    in_pointer.free if in_pointer
  end

  module Unstable
    extend FFI::Library
    ffi_lib File.join(File.expand_path(__dir__), 'blurhash.' + RbConfig::CONFIG['DLEXT'])

    attach_function :BlurhashCompress, %i(pointer int int float pointer), :size_t
    attach_function :BlurhashFreeBuffer, %i(pointer), :void
  end

  private_constant :Unstable
end
