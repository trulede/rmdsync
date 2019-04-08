import pytest
import logging
from time import sleep
import redis


logging.basicConfig(level=logging.DEBUG, format='%(name)s: %(levelname)s: %(message)s (%(module)s:%(funcName)s.%(lineno)d)')
logger = logging.getLogger()


@pytest.mark.incremental
class TestRmdsyncMonitor(object):
    _list = []

    def _handler(self, message):
        self._list.append(message['data'].decode())

    def tXXest__monitor_connect(self, r):
        sync_instance = "stinky_foo"
        sync_config = sync_instance+".config"
        sync_clients = sync_instance+".clients"
        sync_monitor= sync_instance+".monitor"
        client_foo = "foo"


        r.flushall()
        self._list = []

        pubsub = r.pubsub()
        pubsub.channels[sync_monitor.encode()] = self._handler
        pubsub.on_connect(pubsub.connection)
        thread = pubsub.run_in_thread()
        while thread._running is False:
            sleep(0.1)

        r.execute_command("rmdsync.connect", sync_instance, client_foo)
        r.execute_command("rmdsync.disconnect", sync_instance, client_foo)

        pubsub.unsubscribe()
        while thread._running is True:
            sleep(0.1)

        assert self._list == [["stinky_foo:connect:foo"], ["stinky_foo:disconnect:foo"]]
