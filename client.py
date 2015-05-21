import curses
import functools
import re
import socket

import stem
import stem.connection
import stem.socket

from stem.control import EventType, Controller
#from stem.util import str_tools


def main():
  # open main controller
  with Controller.from_port(port = 9151) as controller:
    controller.authenticate()
    controller.set_options({
      '__LeaveStreamsUnattached': '1'
    })

    try:
      curses.wrapper(initialize, controller)
    except KeyboardInterrupt:
      pass


# listen for events and set up callbacks
def initialize(stdscr, controller):
  event_handler = functools.partial(handle_event, stdscr, controller)
  controller.add_event_listener(event_handler, EventType.STREAM)
  stdscr.getch()


# handle a stream event
def handle_event(stdscr, controller, stream):
  p = re.compile('.*\.tor', re.IGNORECASE)
  if p.match(stream.target_address) is not None:
    #stdscr.addstr('[notice] Detected OnioNS domain!\n')

    # send to OnioNS and wait for resolution
    # https://docs.python.org/2/howto/sockets.html
    ipc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    ipc.connect(('localhost', 15678))
    ipc.send(stream.target_address)
    dest = ipc.recv(22)
    ipc.close()

    if dest == 'xxxxxxxxxxxxxxxx.onion'
      dest = '<OnioNS_FAIL>' # triggers fail due to invalid hostname

    r=str(controller.msg('REDIRECTSTREAM ' + stream.id + ' ' + dest))
    stdscr.addstr('[notice] Rewrote ' + stream.target_address + ' to ' + dest + ', ' + r + ' \n')

  if stream.circ_id is None:
    stdscr.addstr('[debug] Attaching request for ' + stream.target_address + ' to circuit\n')
    try:
      controller.attach_stream(stream.id, 0)
    except stem.UnsatisfiableRequest:
      pass

  stdscr.refresh()

if __name__ == '__main__':
  main()
