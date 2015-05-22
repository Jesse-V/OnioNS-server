
import stem, stem.connection, stem.socket
from stem.control import EventType, Controller
#from stem.util import str_tools

import socket, functools, re
from threading import Thread

import time

# start of application
def main():
  # open main controller
  with Controller.from_port(port = 9151) as controller:
    controller.authenticate()
    controller.set_options({
      '__LeaveStreamsUnattached': '1'
    })

    event_handler = functools.partial(handle_event, controller)
    controller.add_event_listener(event_handler, EventType.STREAM)

    time.sleep(60 * 60 * 24 * 365) #basically, wait indefinitely



# handle a stream event
def handle_event(controller, stream):
  print '[debug] ' + str(stream)

  p = re.compile('.*\.tor', re.IGNORECASE)
  if p.match(stream.target_address) is not None: # if .tor, send to OnioNS
    t = Thread(target=resolveOnioNS, args=[controller, stream])
    t.start()
  elif stream.circ_id is None: # if not .tor and unattached, attach now
    attachStream(controller, stream)

  print '[debug] Finished handling stream.'



# resolve via OnioNS a stream's destination
def resolveOnioNS(controller, stream):
  print '[notice] Detected OnioNS domain!'

  # send to OnioNS and wait for resolution
  # https://docs.python.org/2/howto/sockets.html
  ipc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  ipc.connect(('localhost', 9053))
  ipc.send(stream.target_address)
  dest = ipc.recv(22)
  ipc.close()

  if dest == 'xxxxxxxxxxxxxxxx.onion':
    dest = '<OnioNS_FAIL>' # triggers fail due to invalid hostname

  r=str(controller.msg('REDIRECTSTREAM ' + stream.id + ' ' + dest))
  print '[notice] Rewrote ' + stream.target_address + ' to ' + dest + ', ' + r

  attachStream(controller, stream)



# attach the stream to some circuit
def attachStream(controller, stream):
  print '[debug] Attaching request for ' + stream.target_address + ' to circuit'

  try:
    controller.attach_stream(stream.id, 0)
  except stem.UnsatisfiableRequest:
    pass
  except stem.InvalidRequest:
    print '[err] Failed to attach stream. Dropping.'



if __name__ == '__main__':
  main()
