import curses
import functools
import re

import stem
import stem.connection
import stem.socket

from stem.control import EventType, Controller
from stem.util import str_tools

def main():
  # open raw socket with control port
  try:
    csocket = stem.socket.ControlPort(port = 9151)
    stem.connection.authenticate(csocket)
  except stem.SocketError as exc:
    print 'Unable to connect to tor on port 9151: %s' % exc
    sys.exit(1)
  except stem.connection.AuthenticationFailure as exc:
    print 'Unable to authenticate: %s' % exc
    sys.exit(1)

  # open main controller
  with Controller.from_port(port = 9151) as controller:
    controller.authenticate()
    controller.set_options({
      '__LeaveStreamsUnattached': '1'
    })

    try:
      curses.wrapper(initialize, controller, csocket)
    except KeyboardInterrupt:
      pass

# listen for events and set up callbacks
def initialize(stdscr, controller, csocket):
  event_handler = functools.partial(handle_event, stdscr, controller, csocket)
  controller.add_event_listener(event_handler, EventType.STREAM)
  stdscr.getch()

# handle a stream event
def handle_event(stdscr, controller, csocket, stream):
  p = re.compile('.*\.tor', re.IGNORECASE)
  if p.match(stream.target_address) is not None:
    #stdscr.addstr('[notice] Detected OnioNS domain!\n')
    # <lookup here>
    dest='onions55e7yam27n.onion'
    csocket.send('REDIRECTSTREAM ' + stream.id + ' ' + dest)
    r=str(csocket.recv())
    stdscr.addstr('[notice] Rewrote ' + stream.target_address + ' to ' + dest + ' \n')
    #controller.map_address(stream.target_address + '=' + dest)

  if stream.circ_id == None:
    stdscr.addstr('[debug] Attaching request for ' + stream.target_address + ' to circuit\n')
    try:
      controller.attach_stream(stream.id, 0)
    except stem.UnsatisfiableRequest:
      pass

  stdscr.refresh()

if __name__ == '__main__':
  main()
