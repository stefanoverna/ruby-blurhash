require "blurhash/version"
require "ffi"

module Blurhash
  def self.decode(
    blurhash,
    width,
    height,
    punch: 1.0,
    fill_mode: :solid,
    fill_color: [255, 255, 255],
    homogeneous_transform: nil
  )
    out_pointer = FFI::MemoryPointer.new(:pointer)

    fill_color_pointer = if fill_color
                           FFI::MemoryPointer.new(:uint8, 3).tap do |p|
                             p.write_array_of_uint8(fill_color)
                           end
                         end

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
                        end

    out_size_t = Unstable.blurhash_decode(
      blurhash,
      width,
      height,
      punch,
      transform_pointer,
      fill_mode.to_sym,
      fill_color_pointer,
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

    if fill_color_pointer
      fill_color_pointer.free
    end
  end

  module Unstable
    extend FFI::Library
    ffi_lib File.join(File.expand_path(__dir__), 'blurhash.' + RbConfig::CONFIG['DLEXT'])

    enum :fill_mode, [:solid, 1,
                      :blur,
                      :clamp ]

    attach_function :blurhash_decode, %i(string int int float pointer fill_mode pointer pointer), :size_t
    attach_function :blurhash_free, %i(pointer), :void
  end

  private_constant :Unstable
end
