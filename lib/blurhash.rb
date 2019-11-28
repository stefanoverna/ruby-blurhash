require "blurhash/version"
require "ffi"

module Blurhash
  def self.decode(
    blurhash,
    width,
    height,
    punch: 1.0,
    homogeneous_transform: nil
  )
    out_pointer = FFI::MemoryPointer.new(:pointer)

    transform_pointer = if homogeneous_transform
                          FFI::MemoryPointer.new(:float, 6).tap do |p|
                            p.write_array_of_float([
                              homogeneous_transform[0, 0],
                              homogeneous_transform[0, 1],
                              homogeneous_transform[0, 2],
                              homogeneous_transform[1, 0],
                              homogeneous_transform[1, 1],
                              homogeneous_transform[1, 2],
                            ])
                          end
                        else
                          nil
                        end

    out_size_t = Unstable.blurhash_decode(
      blurhash,
      width,
      height,
      punch,
      transform_pointer,
      out_pointer,
    )

    out_result = out_pointer.get_pointer(0)

    result = out_result.read_array_of_uint8(out_size_t)

    Unstable.blurhash_free(out_result)

    result
  ensure
    if transform_pointer
      transform_pointer.free
    end
  end

  module Unstable
    extend FFI::Library
    ffi_lib File.join(File.expand_path(__dir__), 'blurhash.' + RbConfig::CONFIG['DLEXT'])

    attach_function :blurhash_decode, %i(string int int float pointer pointer), :size_t
    attach_function :blurhash_free, %i(pointer), :void
  end

  private_constant :Unstable
end
