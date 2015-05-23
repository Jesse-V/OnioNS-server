
import stem, stem.connection, stem.socket
from stem.control import EventType, Controller

import errno # https://stackoverflow.com/questions/14425401/
from socket import error as socket_error

import socket, functools, re, sys
from threading import Thread

import time

# start of application
def main():
  try:
    # open main controller
    controller = Controller.from_port(port = 9151)
  except stem.SocketError:
    sys.exit("[err] The Tor Browser is not running. Cannot continue")

  controller.authenticate()
  controller.set_options({
    '__LeaveStreamsUnattached': '1'
  })

  print 'Successfully connected to the Tor Browser.'

  event_handler = functools.partial(handle_event, controller)
  controller.add_event_listener(event_handler, EventType.STREAM)

  print 'Now monitoring stream connections.'

  try:
    time.sleep(60 * 60 * 24 * 365) #basically, wait indefinitely
  except KeyboardInterrupt:
    print ''



# handle a stream event
def handle_event(controller, stream):
  print '[debug] ' + str(stream)

  p = re.compile('.*\.tor$', re.IGNORECASE)
  if p.match(stream.target_address) is not None: # if .tor, send to OnioNS
    t = Thread(target=resolveOnioNS, args=[controller, stream])
    t.start()
  elif stream.circ_id is None: # if not .tor and unattached, attach now
    attachStream(controller, stream)

  # print '[debug] Finished handling stream.'



# resolve via OnioNS a stream's destination
def resolveOnioNS(controller, stream):
  print '[notice] Detected OnioNS domain!'

  # send to OnioNS and wait for resolution
  # https://docs.python.org/2/howto/sockets.html
  ipc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

  try:
    ipc.connect(('localhost', 9053))
    ipc.send(stream.target_address)
    dest = ipc.recv(22)
    ipc.close()
  except socket_error as serr:
    if serr.errno != errno.ECONNREFUSED:
      raise serr
    print '[err] OnioNS client is not running!'
    dest = '<IPC_FAIL>'

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
  except stem.InvalidRequest, ir:
    print '[warn] Stream attachment: invalid request. Dropping. '   + ir.message
  except stem.OperationFailed, of:
    print '[warn] Stream attachment: operation failed. Dropping. ' + of.message


if __name__ == '__main__':
  main()
