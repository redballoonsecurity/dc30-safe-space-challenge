import functools
import os
import subprocess

import pytest


def pytest_addoption(parser):
    vanilla_keplers_law_binary = os.path.join(
        os.path.dirname(__file__),
        os.path.pardir,
        "code",
        "build",
        "kepler"
    )
    parser.addoption("--binary", action="store", default=vanilla_keplers_law_binary)


def _calculate_anomaly(eccentricity: float, angle: float, binary: str, ) -> float:
    output: str = subprocess.check_output(
        [binary, str(eccentricity), str(angle)],
        timeout=2
    )
    raw_calculated_anomaly = output.splitlines()[-1]
    return float(raw_calculated_anomaly)


@pytest.fixture
def calculate_anomaly(pytestconfig):
    binary_under_test = pytestconfig.getoption("binary")
    return functools.partial(_calculate_anomaly, binary=binary_under_test)


def _run_binary(binary: str):
    try:
        output: str = subprocess.check_output(
            [binary],
            stderr=subprocess.STDOUT,
            timeout=2
        )
    except subprocess.CalledProcessError as e:
        assert 1 == e.returncode
        output = e.output.decode("ascii")

    assert output.startswith("Usage")
    assert output.endswith("Prints calculated mean_anomaly\n")


@pytest.fixture
def run_binary(pytestconfig):
    binary_under_test = pytestconfig.getoption("binary")
    return functools.partial(_run_binary, binary=binary_under_test)
