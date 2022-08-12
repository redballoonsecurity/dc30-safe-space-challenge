def test_binary_executable(run_binary):
    run_binary()


def test_keplers_law_runs(calculate_anomaly):
    assert 0.0 == calculate_anomaly(0.0, 0.0)


def test_expected_to_complete(calculate_anomaly):
    calculate_anomaly(0.3375, 36.0)


def test_expected_to_hang(calculate_anomaly):
    calculate_anomaly(0.3375, 144.0)
