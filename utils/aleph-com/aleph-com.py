import OSC
import time, threading
import serial
import sys

path = sys.argv[1]
# baud = 115200
baud = 57600
print path
#'/dev/tty.usbmodemfd111'
# path = '/dev/ttyACM0'
receive_address = '127.0.0.1', 12001

ser = serial.Serial()

# OSC Server. there are three different types of server. 
s = OSC.OSCServer(receive_address) # basic
##s = OSC.ThreadingOSCServer(receive_address) # threading
##s = OSC.ForkingOSCServer(receive_address) # forking
c = OSC.OSCClient()
# this registers a 'default' handler (for unmatched messages), 
# an /'error' handler, an '/info' handler.
# And, if the client supports it, a '/subscribe' & '/unsubscribe' handler
#s.addDefaultHandlers()

output = []

def out(data):
  if(data==0 or data == 27):
    output.append(27)
  output.append(data)

# define a message-handler function for the server to call.
def aleph_handler(addr, tags, data, source):
  global output
  output = []
  print "\n\033[92m### osc: %s %s\033[0m" % (addr,data)
  if addr == "/aleph/print":
    output.append(data[0])
  elif addr == "/aleph/param/num":
    output.append(2)
    output.append(0)
  elif addr == "/aleph/param/info":
    output.append(3)
    out(data[0])
    output.append(0)
  elif addr == "/aleph/param/get":
    output.append(4)
    out(data[0])
    output.append(0)
  elif addr == "/aleph/param/set":
    output.append(5)
    i = int(data[1])
    f = int((data[1] % 1) * 65535)
    out(data[0])
    out((i >> 8) & 0xff)
    out(i & 0xff)
    out((f >> 8) & 0xff)
    out(f & 0xff)
    output.append(0)

  if output != []:
    print "\033[94m<--- serial write: %s\033[0m" % (output)
    ser.write(''.join(chr(v) for v in output))

s.addMsgHandler('default', aleph_handler) # adding our function

# just checking which handlers we have added
# print "Registered Callback-functions are :"
# for addr in s.getOSCAddressSpace():
#   print addr

# Start OSCServer
print "\n### ctrl-c to quit."
st = threading.Thread( target = s.serve_forever )
st.start()

c.connect(('0.0.0.0', 12010))

try :
  ser = serial.Serial(path,baud)
  print "\nconnected to %s" % (path) 

  while 1:
    while ser.inWaiting():
      sys.stdout.write(ser.read())

  incoming_bytes = []

  while 1:
    escape = 0
    pos = 0
    while ser.inWaiting():
      i = ord(ser.read())
      # print "incoming: " + str(i)
      if(i==27 and escape ==0): escape = 1
      elif(i==0 and escape ==0):
        n=0

        if(incoming_bytes == []): incoming_bytes.append(0)

        if(incoming_bytes[0]==1):     #debug
          n = 1;
          while(n<pos):
            sys.stdout.write(chr(incoming_bytes[n]))
            n += 1
          sys.stdout.flush()

        elif(incoming_bytes[0]==2):   #param/num
          print "\n\033[94m---> number of params " + str(incoming_bytes[1]) + "\033[0m"
          m = OSC.OSCMessage()
          m.setAddress("/aleph/param/num")
          m.append(incoming_bytes[1])
          try:
            c.send(m)
          except OSCClientError:
            pass
        elif(incoming_bytes[0]==3):   #param/info
          print "\n\033[94m---> param info for index " + str(incoming_bytes[1]) + "\033[0m"
          sep = incoming_bytes.index(31);
          stt = incoming_bytes[2:sep]
          stt = [chr(i) for i in stt]
          m = OSC.OSCMessage()
          m.setAddress("/aleph/param/info")
          m.append(incoming_bytes[1])
          m.append(''.join(stt))
          try:
            c.send(m)
          except OSCClientError:
            pass
        elif(incoming_bytes[0]==4):   #param/get
          print "\n\033[94m---> param value for index " + str(incoming_bytes[1]) + "\033[0m"
          val = (incoming_bytes[2] << 8) + (incoming_bytes[3]) + (((incoming_bytes[4] << 8) + (incoming_bytes[5])) / 65535.0)
          m = OSC.OSCMessage()
          m.setAddress("/aleph/param/get")
          m.append(incoming_bytes[1])
          m.append(val)
          try:
            c.send(m)
          except OSCClientError:
            pass

        incoming_bytes = []
        pos = 0
      elif(i == 31 and escape == 1):
        # separator
        incoming_bytes.append(27)
        incoming_bytes.append(31)
        pos += 2
        escape = 0
      else:
        incoming_bytes.append(i)
        pos += 1
        escape = 0

    time.sleep(0.01)

except serial.serialutil.SerialException:
    print "didn't connect"

except OSError:
    print "no serial?"

except IOError:
  print "%s: device disconnected" % path

except KeyboardInterrupt :
  print "quit"

print "Closing OSCServer."
s.close()
print "Waiting for Server-thread to finish"
st.join() ##!!!
print "Done"

