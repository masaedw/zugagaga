#!/usr/bin/ruby
# last modified <2004/01/22 02:53:43 2004 JST>

module State
  START_DATA = "START_DATA"
  MSG_HEAD = "MSG_HEAD"
  SERVER_EVENT = "SERVER_EVENT"
  SOA_SIZE = "SOA_SIZE"
  SEND_OBJECT = "SEND_OBJECT"
  SEND_PARAM = "SEND_PARAM"
  END_DATA = "END_DATA"
  ERROR_DATA = "ERROR_DATA"
  EXIT_PARSE = "EXIT_PARSE"
end

class String
  def bin
    ("0b" + self).oct
  end
end

class HogeP
  include State

  MSG_HEAD_EXP = /000000001111111100000000111111110000000011111111/

  def initialize(bits)
    @bits = bits
    @bytes = bits.scan(/.{8}/)
    initAttr
  end

  private
  def initAttr
    @readp = 0
    @state = START_DATA
    @soasize = 0
    @paramsize = 0
  end

  public
  def nextData
    @state = END_DATA if @readp >= @bytes.size - 1
    case @state
    when START_DATA
      msgheadp = searchMsgHead(0)
      if msgheadp
	@readp = msgheadp
	@state = MSG_HEAD
      else
	@state = ERROR_DATA
      end
    when MSG_HEAD
      printBytes(@bytes[@readp,6], @readp, "MSG_HEAD")
      @readp += 6
      @state = SERVER_EVENT
    when SERVER_EVENT
      byte = @bytes[@readp]
      opt = nil
      if byte == "00000001"
	opt = "ServerEvent::Update"
	@state = SOA_SIZE
      else
	@state = ERROR_DATA
	return self
      end
      printBytes([@bytes[@readp]], @readp, opt)
      @readp += 1
    when SOA_SIZE
      bytes = @bytes[@readp,4]
      @soasize = bytes.join.bin
      printBytes(bytes, @readp, "SendObjAry size: #{@soasize}")
      @readp += 4
      @state = SEND_OBJECT
    when SEND_OBJECT
      @soasize.times do
	getNextSendObject
	return self if endData?
      end
      @state = SEND_PARAM
    when SEND_PARAM
      if @readp + 7 > @bytes.size
	@state = ERROR_DATA
	return self
      end

      x1, x2, y1, y2, size1, size2, dash = @bytes[@readp,7]
      x = (x1+x2).bin
      y = (y1+y2).bin
      size = (size1+size2).bin
      printBytes(@bytes[@readp,7], @readp, "SendParam x: #{x}, y: #{y}, size: #{size}, dash: #{dash.bin}")
      @readp += 7
      if ( @readp != @bytes.size - 1 )
	t = searchMsgHead(@readp)
	if ( !t )
	  @state = ERROR_DATA
	elsif ( t == @readp )
	  @state = MSG_HEAD
	else
	  printBytes(@bytes[@readp,t-@readp], @readp, "????")
	  @readp = t
	  @state = MSG_HEAD
	end
      else
	END_DATA
      end
    when ERROR_DATA
      errPrint
      @state = EXIT_PARSE
    when END_DATA
      @state = EXIT_PARSE
    end

    self
  end

  private
  def getNextSendObject
    if @readp + 6 >= @bytes.size
      @state = ERROR_DATA
      return
    end

    x1, x2, y1, y2, type, state = @bytes[@readp,6]
    pos = @readp
    @readp += 6
    x = (x1+x2).bin
    y = (y1+y2).bin
    printBytes(@bytes[pos,6], pos, "Object x: #{x}, y: #{y}, type: #{type.bin}, state: #{state.bin}")
  end

  def searchMsgHead( a_readp )
    bytestr = ""
    readp = a_readp
    loop do
      break if readp >= @bytes.size - 1
      bytestr += @bytes[readp]
      readp += 1
      if MSG_HEAD_EXP =~ bytestr
	return readp - 6
      end
    end
    return false
  end

  public
  def printAll
    print "----------------------Parse Data Stream\n"
    while !finished?
      nextData
    end
    print "----------------------End of Data (#{@bytes.size} bytes).\n\n"
  end

  def endData?
    @state == END_DATA
  end

  def finished?
    @state == EXIT_PARSE
  end

  def rewind
    initAttr
  end

  private
  def printBytes(bytes, start, optstring, endline=true)
    bytes.each_with_index do |byte,i|
      b10 = byte.bin
      opt = if i == bytes.size - 1
	      "  " + optstring + (endline ? "\n" : "")
	    else
	      ""
	    end
      printf("%2d %s %02X %3d%s\n", start+i, byte, b10, b10, opt)
    end
  end

  def errPrint
    printBytes(@bytes[@readp..-1], @readp, "Error")
  end
end

def testmain_
  filename = if ARGV.size >= 1
               ARGV[0]
             else
               print "select log file name\n"
               exit
             end

  File.open(filename) do |file|
    file.each_line do |line|
      if ( /data: / =~ line )
        bits = line.sub(/.*data: /, "")
        HogeP.new(bits).printAll
        #exit
      else
        puts line
      end
    end
  end
end

if $0 == __FILE__
  begin
    testmain_
  rescue Errno::EPIPE
  end
end
