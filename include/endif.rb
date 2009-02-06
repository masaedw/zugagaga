#!/usr/bin/ruby -Ke

ARGV.each {|name|
  next unless File.exist?(name)
  hoge = ""
  flag = 0
  fileno = 0
  file = File.readlines(name)
  file.each {|line|
    case line
    when /#ifndef (.*)/
      hoge = $1 if flag == 0
    when /#endif/
      if flag == 0
	line.sub!(/.*/) { "#endif // #{hoge}" }
	break
      end
    end
  }
  File.open(name, "w"){|i| i.write(file.join)}
}
