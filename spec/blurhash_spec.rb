# frozen_string_literal: true

require "spec_helper"

RSpec.describe Blurhash do
  it "has a version number" do
    expect(Blurhash::VERSION).not_to be nil
  end

  subject(:result) do
    Blurhash.decode(
      blurhash,
      width,
      height,
      punch: punch,
      saturation: saturation,
    )
  end

  let(:blurhash) do
    "LGFFaXYk^6#M@-5c,1J5@[or[Q6."
  end

  let(:width) { 2 }
  let(:height) { 2 }
  let(:punch) { 1.0 }
  let(:saturation) { 0 }

  it "does something useful" do
    expect(result).to eq([177, 118, 164, 119, 100, 170, 138, 155, 178, 108, 126, 131])
  end

  describe "saturation" do
    let(:blurhash) do
      "L-N^3rxu_NWB?bt7ofWCbcRjaJs."
    end

    let(:width) { 30 }
    let(:height) { 30 }
    let(:punch) { 1.0 }
    let(:saturation) { 100 }

    def write(path, bytes)
      File.open(path, "w") do |f|
        f.puts "P6", "#{width} #{height}", "255"
        f.binmode
        f.print(bytes.pack("C*"))
      end
    end

    context "sat = -100" do
      let(:saturation) { -100 }

      it "works" do
        write("-100.ppm", result)
      end
    end

    context "sat = 0" do
      let(:saturation) { 0 }

      it "works" do
        write("0.ppm", result)
      end
    end

    context "sat = 100" do
      let(:saturation) { 100 }

      it "works" do
        write("100.ppm", result)
      end
    end
  end
end
