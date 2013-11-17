import OSC
import time, threading
import serial
import sys

path = '/dev/cu.usbmodemfd111'
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
  if addr == "/aleph/param/count":
    output.append(2)
    output.append(0)
  elif addr == "/aleph/param/info":
    output.append(3)
    out(data[0])
    output.append(data[0])
    output.append(0)
  elif addr == "/aleph/param/get":
    output.append(4)
    out(data[0])
    output.append(0)
  elif addr == "/aleph/param/set":
    output.append(5)
    out(data[0])
    out(data[1])
    out(data[2])
    out(data[3])
    out(data[4])
    output.append(0)

  if output != []:
    print "\033[94m--- serial write: %s\033[0m" % (output)
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
  ser = serial.Serial(path,500000)
  print "\nconnected to %s" % (path) 

  incoming_bytes = []

  while 1:
    while ser.inWaiting():
      i = ser.read()
      sys.stdout.write(i)
      sys.stdout.flush()
      # incoming_bytes.append(ser.read())
      # print "serial received: %s" % (incoming_bytes[0])

      # m = OSC.OSCMessage()

      # m.setAddress("/debug")
      # m.append(ord(incoming_bytes[0]))
        
      # try:
      #   c.send(m)
      # except OSCClientError:
      #   pass
      #   incoming_bytes = []
    time.sleep(0.1)

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

