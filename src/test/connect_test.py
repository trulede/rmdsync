import pytest
import logging
from time import sleep
import redis
from redis import ResponseError


logging.basicConfig(level=logging.DEBUG, format='%(name)s: %(levelname)s: %(message)s (%(module)s:%(funcName)s.%(lineno)d)')
logger = logging.getLogger()


@pytest.mark.incremental
class TestRmdsyncConnect(object):

    def test__connect(self, r):
        sync_instance = "stinky_foo"
        sync_config = sync_instance+".config"
        sync_clients = sync_instance+".clients"
        client_foo = "foo"
        client_bar = "bar"


        r.flushall()
        assert r.hlen(sync_config) == 0
        assert r.scard(sync_clients) == 0


        rc = r.execute_command("rmdsync.connect", sync_instance, client_foo)
        assert rc == "OK"
        assert r.hlen(sync_config) >= 2
        assert r.hget(sync_config, "name") == sync_instance
        assert r.hget(sync_config, "timebase_us") == "5000" # Default to 5 mS
        assert r.scard(sync_clients) == 1
        assert r.sismember(sync_clients, client_foo) == 1

        rc = r.execute_command("rmdsync.connect", sync_instance, client_bar)
        assert rc == "OK"
        assert r.hlen(sync_config) >= 2
        assert r.hget(sync_config, "name") == sync_instance
        assert r.hget(sync_config, "timebase_us") == "5000" # Default to 5 mS
        assert r.scard(sync_clients) == 2
        assert r.sismember(sync_clients, client_foo) == 1
        assert r.sismember(sync_clients, client_bar) == 1

        rc = r.execute_command("rmdsync.disconnect", sync_instance, client_foo)
        assert rc == "OK"
        assert r.hlen(sync_config) >= 2
        assert r.hget(sync_config, "name") == sync_instance
        assert r.hget(sync_config, "timebase_us") == "5000" # Default to 5 mS
        assert r.scard(sync_clients) == 0
        assert r.sismember(sync_clients, client_foo) == 0
        assert r.sismember(sync_clients, client_bar) == 1

        rc = r.execute_command("rmdsync.disconnect", sync_instance, client_bar)
        assert r.hlen(sync_config) >= 2
        assert r.hget(sync_config, "name") == sync_instance
        assert r.hget(sync_config, "timebase_us") == "5000" # Default to 5 mS
        assert r.scard(sync_clients) == 0


    def test__reconnect(self, r):
        sync_instance = "stinky_foo"
        sync_config = sync_instance+".config"
        sync_clients = sync_instance+".clients"
        client_foo = "foo"


        r.flushall()
        assert r.hlen(sync_config) == 0
        assert r.scard(sync_clients) == 0


        rc = r.execute_command("rmdsync.connect", sync_instance, client_foo)
        assert rc == "OK"
        assert r.hlen(sync_config) >= 2
        assert r.hget(sync_config, "name") == sync_instance
        assert r.hget(sync_config, "timebase_us") == "5000" # Default to 5 mS
        assert r.scard(sync_clients) == 1
        assert r.sismember(sync_clients, client_foo) == 1

        r.hset(sync_config, "timebase_us", "123456")
        assert r.hget(sync_config, "timebase_us") == "123456"

        rc = r.execute_command("rmdsync.disconnect", sync_instance, client_foo)
        assert rc == "OK"
        assert r.hlen(sync_config) >= 2
        assert r.hget(sync_config, "name") == sync_instance
        assert r.hget(sync_config, "timebase_us") == "123456"
        assert r.scard(sync_clients) == 0

        rc = r.execute_command("rmdsync.connect", sync_instance, client_foo)
        assert rc == "OK"
        assert r.hlen(sync_config) >= 2
        assert r.hget(sync_config, "name") == sync_instance
        assert r.hget(sync_config, "timebase_us") == "123456"
        assert r.scard(sync_clients) == 1
        assert r.sismember(sync_clients, client_foo) == 1
