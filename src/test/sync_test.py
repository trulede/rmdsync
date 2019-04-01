import pytest
import logging
from time import sleep
from threading import Thread
import redis


logging.basicConfig(level=logging.DEBUG, format='%(name)s: %(levelname)s: %(message)s (%(module)s:%(funcName)s.%(lineno)d)')
logger = logging.getLogger()


@pytest.mark.incremental
class TestRmdsyncSync(object):

    def sync_thread(r, sync_instance, client, wakeup, resp):
        # Call Sync with paramter 0 and wait for a signal.
        resp[0] = [ 0, 0 ]
        t = time()
        # Block until some other client releases the Sync Instance.
        rc = r.execute_command("rmdsync.sync", sync_instance, client, wakeup)
        resp[0][0] = time() - t
        resp[0][1] = rc

    def signal_thread(r, sync_instance, delay, resp):
        resp[0] = [ 0, 0 ]
        t = time()
        rc = r.execute_command("rmdsync.signal", sync_instance)
        resp[0][0] = time() - t
        resp[0][1] = rc


    def test__sync(self, r):
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


        def do_sync_and_check(r, sync_instance, client, wakeup, delay, expected_time, expected_interval):
            resp = [None]
            t = Thread(target = sync_thread, args = (r, sync_instance, client, wakeup, resp))
            t.start()
            sleep(delay)
            t.join(timeout = 10)
            assert resp[0][1][0] == expected_time
            assert resp[0][1][1] == expected_interval


        # -1, initial condition - 0 0.
        do_sync_and_check(y, sync_instance, client_foo, -1, 0.1, 0, 0)
        # x uS, a few cycles.
        do_sync_and_check(y, sync_instance, client_foo, 4000, 0.1, 4000, 4000)
        do_sync_and_check(y, sync_instance, client_foo, 5000, 0.1, 9000, 5000)
        do_sync_and_check(y, sync_instance, client_foo, 6000, 0.1, 15000, 6000)
        do_sync_and_check(y, sync_instance, client_foo, -1, 0.1, 15000, 0)
        # 0, default cycles: see keyspace, will default to 5000
        do_sync_and_check(y, sync_instance, client_foo, 0, 0.1, 20000, 5000)
        do_sync_and_check(y, sync_instance, client_foo, 0, 0.1, 25000, 5000)
        do_sync_and_check(y, sync_instance, client_foo, 0, 0.1, 30000, 5000)


        rc = r.execute_command("rmdsync.disconnect", sync_instance, client_foo)
        assert r.hlen(sync_config) >= 2
        assert r.hget(sync_config, "name") == sync_instance
        assert r.hget(sync_config, "timebase_us") == "5000"
        assert r.scard(sync_clients) == 0



    def test__sync_signal(self, r):
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


        foo_resp = [None]
        foo_t = Thread(target = sync_thread, args = (r, sync_instance, client_foo, 2000000, foo_resp))
        foo_t.start()
        sleep(0.1)

        rc = r.execute_command("rmdsync.signal", sync_instance)
        assert rc[0] == 1
        assert rc[1] == 1
        assert rc[2] > 90000  # 90 mS, less than the sleep time
        assert rc[2] < 120000 # 120 mS, more than the sleep time
        foo_t.join(timeout = 10)
        assert foo_resp[0][0] > 0
        assert foo_resp[0][1][0] == rc[2] # Sync Instance time.
        assert foo_resp[0][1][1] == rc[2] # Sync Instance time since last release (which was t = 0).


        rc = r.execute_command("rmdsync.disconnect", sync_instance, client_foo)
        assert r.scard(sync_clients) == 0



    def test__multiple_sync_clients(self, r):
        sync_instance = "stinky_foo"
        sync_config = sync_instance+".config"
        sync_clients = sync_instance+".clients"
        client_foo = "foo"
        client_foo = "bar"

        r.flushall()
        assert r.hlen(sync_config) == 0
        assert r.scard(sync_clients) == 0

        r.execute_command("rmdsync.connect", sync_instance, client_foo)
        r.execute_command("rmdsync.connect", sync_instance, client_bar)
        assert r.scard(sync_clients) == 2
        assert r.sismember(sync_clients, client_foo) == 1
        assert r.sismember(sync_clients, client_bar) == 1

        # Foo wakes in 2000 mS, Bar wakes in 5 mS: both should wait in 5 mS
        foo_resp = [None]
        bar_resp = [None]
        foo_t = Thread(target = sync_thread, args = (r, sync_instance, client_foo, 2000000, foo_resp))
        bar_t = Thread(target = sync_thread, args = (r, sync_instance, client_bar, 5000, bar_resp))
        foo_t.start()
        bar_t.start()
        sleep(0.1)
        foo_t.join(timeout = 10)
        bar_t.join(timeout = 10)
        assert foo_resp[0][1][0] == 5000 # Sync Instance time.
        assert foo_resp[0][1][1] == 5000
        assert bar_resp[0][1][0] == 5000 # Sync Instance time.
        assert bar_resp[0][1][1] == 5000

        # Bar wakes at default time (5000 uS)
        foo_resp = [None]
        bar_resp = [None]
        foo_t = Thread(target = sync_thread, args = (r, sync_instance, client_foo, 2000000, foo_resp))
        bar_t = Thread(target = sync_thread, args = (r, sync_instance, client_bar, 0, bar_resp))
        foo_t.start()
        bar_t.start()
        sleep(0.1)
        foo_t.join(timeout = 10)
        bar_t.join(timeout = 10)
        assert foo_resp[0][1][0] == 10000 # Sync Instance time.
        assert foo_resp[0][1][1] == 5000
        assert bar_resp[0][1][0] == 10000 # Sync Instance time.
        assert bar_resp[0][1][1] == 5000

        # Foo disconnects, Bar runs again
        rc = r.execute_command("rmdsync.disconnect", sync_instance, client_foo)
        assert r.scard(sync_clients) == 1
        assert r.sismember(sync_clients, client_foo) == 0
        assert r.sismember(sync_clients, client_bar) == 1
        bar_resp = [None]
        bar_t = Thread(target = sync_thread, args = (r, sync_instance, client_bar, 5000, bar_resp))
        bar_t.start()
        sleep(0.1)
        bar_t.join(timeout = 10)
        assert bar_resp[0][1][0] == 15000 # Sync Instance time.
        assert bar_resp[0][1][1] == 5000


        rc = r.execute_command("rmdsync.disconnect", sync_instance, client_bar)
        assert r.scard(sync_clients) == 0
        assert r.sismember(sync_clients, client_foo) == 0
        assert r.sismember(sync_clients, client_bar) == 0



    def test__sync_time_on_blocking_sync_instance(self, r):
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


        foo_resp = [None]
        foo_t = Thread(target = sync_thread, args = (r, sync_instance, client_foo, 500000, foo_resp))
        foo_t.start()
        sleep(0.1)

        # Call sync with -1 to get the Sync Instance time ... which should be between 0 and 500000
        rc = r.execute_command("rmdsync.sync", sync_instance, client_foo, -1)
        assert rc[0] > 90000  # 90 mS, less than the sleep time
        assert rc[0] < 120000 # 120 mS, more than the sleep time
        assert rc[1] == 0

        foo_t.join(timeout = 10)
        rc = r.execute_command("rmdsync.disconnect", sync_instance, client_foo)
        assert r.scard(sync_clients) == 0
