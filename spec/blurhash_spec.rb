# frozen_string_literal: true

require "spec_helper"

RSpec.describe Blurhash do
  it "has a version number" do
    expect(Blurhash::VERSION).not_to be nil
  end

  let(:blurhash) do
    "LGFFaXYk^6#M@-5c,1J5@[or[Q6."
  end

  let(:width) { 2 }
  let(:height) { 2 }
  let(:punch) { 1.0 }

  it "does something useful" do
    expect(
      Blurhash.decode(
        blurhash,
        width,
        height,
        punch: punch,
        saturation: 0,
      ),
    ).to eq([177, 118, 164, 119, 100, 170, 138, 155, 178, 108, 126, 131])
  end
end
