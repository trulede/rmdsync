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
        sync_instance = b'stinky_foo'
        sync_config = b'stinky_foo.config'
        sync_clients = b'stinky_foo.clients'
        sync_thread = b'stinky_foo.thread'
        client_foo = b'foo'
        client_bar = b'bar'


        r.flushall()
        assert r.hlen(sync_config) == 0
        assert r.scard(sync_clients) == 0
        assert not r.exists(sync_thread)


        rc = r.execute_command("rmdsync.connect", sync_instance, client_foo)
        assert rc == b'OK'
        assert r.hlen(sync_config) >= 2
        assert r.hget(sync_config, "name") == sync_instance
        assert r.hget(sync_config, "timebase_us") == b'5000' # Default to 5 mS
        assert r.hget(sync_config, "sample_freq_us") == b'50'
        assert r.scard(sync_clients) == 1
        assert r.sismember(sync_clients, client_foo) == 1
        assert r.exists(sync_thread)
        thread_id = r.get(sync_thread)

        rc = r.execute_command("rmdsync.connect", sync_instance, client_bar)
        assert rc == b'OK'
        assert r.hlen(sync_config) >= 2
        assert r.hget(sync_config, "name") == sync_instance
        assert r.hget(sync_config, "timebase_us") == b'5000' # Default to 5 mS
        assert r.hget(sync_config, "sample_freq_us") == b'50'
        assert r.scard(sync_clients) == 2
        assert r.sismember(sync_clients, client_foo) == 1
        assert r.sismember(sync_clients, client_bar) == 1
        assert r.exists(sync_thread)
        assert r.get(sync_thread) == thread_id

        rc = r.execute_command("rmdsync.disconnect", sync_instance, client_foo)
        assert rc == b'OK'
        assert r.hlen(sync_config) >= 2
        assert r.hget(sync_config, "name") == sync_instance
        assert r.hget(sync_config, "timebase_us") == b'5000' # Default to 5 mS
        assert r.hget(sync_config, "sample_freq_us") == b'50'
        assert r.scard(sync_clients) == 1
        assert r.sismember(sync_clients, client_foo) == 0
        assert r.sismember(sync_clients, client_bar) == 1
        assert r.exists(sync_thread)
        assert r.get(sync_thread) == thread_id

        rc = r.execute_command("rmdsync.disconnect", sync_instance, client_bar)
        assert r.hlen(sync_config) >= 2
        assert r.hget(sync_config, "name") == sync_instance
        assert r.hget(sync_config, "timebase_us") == b'5000' # Default to 5 mS
        assert r.hget(sync_config, "sample_freq_us") == b'50'
        assert r.scard(sync_clients) == 0
        assert not r.exists(sync_thread)


    def test__reconnect(self, r):
        sync_instance = b'stinky_foo'
        sync_config = b'stinky_foo.config'
        sync_clients = b'stinky_foo.clients'
        sync_thread = b'stinky_foo.thread'
        client_foo = b'foo'

        r.flushall()
        assert r.hlen(sync_config) == 0
        assert not r.exists(sync_thread)
        assert r.scard(sync_clients) == 0


        rc = r.execute_command("rmdsync.connect", sync_instance, client_foo)
        assert rc == b'OK'
        assert r.hlen(sync_config) >= 2
        assert r.hget(sync_config, "name") == sync_instance
        assert r.hget(sync_config, "timebase_us") == b'5000' # Default to 5 mS
        assert r.hget(sync_config, "sample_freq_us") == b'50'
        assert r.scard(sync_clients) == 1
        assert r.sismember(sync_clients, client_foo) == 1
        assert r.exists(sync_thread)

        r.hset(sync_config, "timebase_us", b'123456')
        r.hset(sync_config, "sample_freq_us", b'100')
        assert r.hget(sync_config, "timebase_us") == b'123456'
        assert r.hget(sync_config, "sample_freq_us") == b'100'

        rc = r.execute_command("rmdsync.disconnect", sync_instance, client_foo)
        assert rc == b'OK'
        assert r.hlen(sync_config) >= 2
        assert r.hget(sync_config, "name") == sync_instance
        assert r.hget(sync_config, "timebase_us") == b'123456'
        assert r.hget(sync_config, "sample_freq_us") == b'100'
        assert r.scard(sync_clients) == 0
        assert not r.exists(sync_thread)

        rc = r.execute_command("rmdsync.connect", sync_instance, client_foo)
        assert rc == b'OK'
        assert r.hlen(sync_config) >= 2
        assert r.hget(sync_config, "name") == sync_instance
        assert r.hget(sync_config, "timebase_us") == b'123456'
        assert r.hget(sync_config, "sample_freq_us") == b'100'
        assert r.scard(sync_clients) == 1
        assert r.sismember(sync_clients, client_foo) == 1
        assert r.exists(sync_thread)

        rc = r.execute_command("rmdsync.disconnect", sync_instance, client_foo)
        assert rc == b'OK'
        assert r.scard(sync_clients) == 0
        assert not r.exists(sync_thread)


    def test__(self, r):
        sync_foo = b'foo'
        sync_bar = b'bar'

        r.flushall()

        def _connect(r, sync):
            sync_instance = '{}'.format(sync).encode()
            sync_config = '{}.config'.format(sync).encode()
            sync_clients = '{}.clients'.format(sync).encode()
            sync_thread = '{}.thread'.format(sync).encode()

            rc = r.execute_command("rmdsync.connect", sync_instance, sync)
            assert rc == b'OK'
            assert r.hget(sync_config, "name") == sync_instance
            assert r.hget(sync_config, "timebase_us") == b'5000'
            assert r.hget(sync_config, "sample_freq_us") == b'50'
            assert r.scard(sync_clients) == 1
            assert r.sismember(sync_clients, sync) == 1
            assert r.exists(sync_thread)
            thread_id = r.get(sync_thread)

            return thread_id

        def _disconnect(r, sync):
            sync_instance = '{}'.format(sync).encode()
            sync_config = '{}.config'.format(sync).encode()
            sync_clients = '{}.clients'.format(sync).encode()
            sync_thread = '{}.thread'.format(sync).encode()

            rc = r.execute_command("rmdsync.disconnect", sync_instance, sync)
            assert rc == b'OK'
            assert r.scard(sync_clients) == 0
            assert not r.exists(sync_thread)


        foo_thread_id = _connect(r, b'foo')
        bar_thread_id = _connect(r, b'bar')
        assert foo_thread_id != bar_thread_id
        _disconnect(r, b'foo')
        _disconnect(r, b'bar')
