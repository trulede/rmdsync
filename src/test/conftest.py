import pytest
import logging
import redis



logging.basicConfig(level=logging.DEBUG, format='%(name)s: %(levelname)s: %(message)s (%(module)s:%(funcName)s.%(lineno)d)')
logger = logging.getLogger()


def pytest_runtest_makereport(item, call):
    if "incremental" in item.keywords:
        if call.excinfo is not None:
            parent = item.parent
            parent._previousfailed = item

def pytest_runtest_setup(item):
    if "incremental" in item.keywords:
        previousfailed = getattr(item.parent, "_previousfailed", None)
        if previousfailed is not None:
            pytest.xfail("previous test failed (%s)" %previousfailed.name)

@pytest.fixture(scope="module")
def r():
    redis_url = os.getenv('REDIS_URL', "redis://localhost:6379")
    r = redis.from_url(redis_url)
    r.client_setname("foobar")
    logger.info('Redis : {}'.format(r.client_getname()))
    yield r
    logger.info('Redis : {}'.format(r.client_getname()))
