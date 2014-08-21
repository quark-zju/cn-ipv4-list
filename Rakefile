require 'rake/clean'

APNIC_LIST_FILE = 'delegated-apnic-latest'
LIST_FILE       = 'cn-ipv4-list.txt'
INV_LIST_FILE   = LIST_FILE.sub(/\.txt/, '-inverted\0')
INV_BIN         = 'ipv4inv'
CXX             = ENV['CXX'] || 'g++'
CXXFLAGS        = [ENV['CXXFLAGS'], ENV['CFLAGS'], '-std=c++11'].compact * ' '

CLEAN.include(INV_BIN)
CLOBBER.include('*.txt')

desc 'Download latest ipv4 list from APNIC.'
file APNIC_LIST_FILE do |t|
  system 'wget -c http://ftp.apnic.net/apnic/stats/apnic/delegated-apnic-latest'
end

desc 'Generate cn ipv4 list from downloaded APNIC list.'
file LIST_FILE => APNIC_LIST_FILE do |t|
  File.open(LIST_FILE, 'w') do |o|
    File.read(APNIC_LIST_FILE).each_line do |l|
      next if !l.include?('ipv4') || l.include?('*')
      cc, type, start, value = l.split('|')[1, 4]
      next if cc != 'CN'
      prefix = 32 - Math.log2(value.to_i).to_i
      o.puts "#{start}/#{prefix}"
    end
  end
end

file INV_BIN => "#{INV_BIN}.cc" do |t|
  sh CXX, CXXFLAGS, '-o', t.name, CXXFLAGS, *t.prerequisites
end

desc 'Generate inverted list.'
file INV_LIST_FILE => [LIST_FILE, INV_BIN] do |t|
  sh "./#{INV_BIN} < #{LIST_FILE} > #{INV_LIST_FILE}"
end

