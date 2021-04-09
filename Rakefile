# frozen_string_literal: true

require "bundler/gem_tasks"
require "rake/extensiontask"
require "rspec/core/rake_task"

RSpec::Core::RakeTask.new(:spec)

task default: :spec

Rake::ExtensionTask.new "blurhash" do |ext|
  ext.lib_dir = "lib"
end
