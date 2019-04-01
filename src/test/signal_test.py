import pytest
import logging
from time import sleep
from threading import Thread
import redis


logging.basicConfig(level=logging.DEBUG, format='%(name)s: %(levelname)s: %(message)s (%(module)s:%(funcName)s.%(lineno)d)')
logger = logging.getLogger()


@pytest.mark.incremental
class TestRmdsyncSignal(object):

    def sync_thread(r, sync_instance, client, resp):
        # Call Sync with paramter 0 and wait for a signal.
        resp[0] = [ 0, 0 ]
        t = time()
        # Block until some other client releases the Sync Instance.
        rc = r.execute_command("rmdsync.sync", sync_instance, client, 0)
        resp[0][0] = time() - t
        resp[0][1] = rc


    def test__signal_no_client():
        sync_instance = "stinky_foo"
        sync_config = sync_instance+".config"
        sync_clients = sync_instance+".clients"
        client_foo = "foo"


        r.flushall()
        assert r.hlen(sync_config) == 0
        assert r.scard(sync_clients) == 0

        assert r.scard(sync_clients) == 0
        assert r.sismember(sync_clients, client_foo) == 0

        rc = r.execute_command("rmdsync.signal", sync_instance)
        assert rc[0] == 0
        assert rc[1] == 0
        assert rc[2] == 0 # Sync Instance time should be 0? (since it does not exist)


    def test__signal(self, r):
        sync_instance = "stinky_foo"
        sync_config = sync_instance+".config"
        sync_clients = sync_instance+".clients"
        client_foo = "foo"


        r.flushall()
        assert r.hlen(sync_config) == 0
        assert r.scard(sync_clients) == 0

        r.execute_command("rmdsync.connect", sync_instance, client_foo)
        assert r.scard(sync_clients) == 1
        assert r.sismember(sync_clients, client_foo) == 1

        foo_t0 = time()
        foo_resp = [None]
        foo_t = Thread(target = sync_thread, args = (r, sync_instance, client_foo, foo_resp))
        foo_t.start()

        sleep(0.1)
        rc = r.execute_command("rmdsync.signal", sync_instance)
        assert rc[0] == 1
        assert rc[1] == 1
        assert rc[2] > 0 # Sync Instance time

        foo_t.join(timeout = 10)
        foo_t1 = time()
        assert foo_resp[0][0] > 0
        assert foo_resp[0][1][0] == rc[2] # Sync Instance time.
        assert foo_resp[0][1][1] == rc[2] # Sync Instance time since last release (which was t = 0).

        rc = r.execute_command("rmdsync.disconnect", sync_instance, client_foo)
        assert r.hlen(sync_config) >= 2
        assert r.hget(sync_config, "name") == sync_instance
        assert r.hget(sync_config, "timebase_us") == "5000" # Default to 5 mS
        assert r.scard(sync_clients) == 0
