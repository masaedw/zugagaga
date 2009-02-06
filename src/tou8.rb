ARGV.each do |i|
  `lv -Iu8 -Ou8 #{i} > #{i+"tmp"}`
  `mv -f #{i+"tmp"} #{i}`
end
