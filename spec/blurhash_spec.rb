require "spec_helper"

RSpec.describe Blurhash do
  it "has a version number" do
    expect(Blurhash::VERSION).not_to be nil
  end

  let(:pixels) do
    [255, 0, 0]
  end

  let(:width) { 1 }
  let(:height) { 1 }

  it "does something useful" do
    expect(Blurhash.compress(pixels, width, height)).to eq("data:image/jpg;base64,/9j/4AAQSkZJRgABAQAAAQABAAD/2wCEAAoHBwgHBgoICAgFCgoFBQwFBQUFBREJCgUMFxMZGBYTFhUaHysjGh0oHRUWJDUlKC0vMjIyGSI4PTcwPCsxMi8BCgsLBQUFEAUFEC8cFhwvLy8vLy8vLy8vLy8vLy8vLy8vLy8vLy8vLy8vLy8vLy8vLy8vLy8vLy8vLy8vLy8vL//AABEIAAEAAQMBIgACEQEDEQH/xAAVAAEBAAAAAAAAAAAAAAAAAAAABv/EABQQAQAAAAAAAAAAAAAAAAAAAAD/xAAVAQEBAAAAAAAAAAAAAAAAAAAHBv/EABQRAQAAAAAAAAAAAAAAAAAAAAD/2gAMAwEAAhEDEQA/AJUBOmZ//9k=")
  end
end
