# DEFCON 30 Aerospace Village CTF
# Safe Space

This challenge is about using binary patching techniques to solve a real-world problem. Let's say there is a bug in a spacecraft's firmware. In this environment, upload bandwith is extremely limited, so there are two objectives:

1. Effectively patch the bug

2. Efficiently deliver this patch

For this challenge, it would be relatively easy to meet the first objective by patching the C source code, recompiling, and uploading the new code. But the second objective is harder to meet. That is the core of this challenge.

Red Balloon Security, Inc. developed this challenge for DEFCON 30 based on a [challenge developed by NASA JPL](https://github.com/Assured-Micropatching/Practice-Problems/tree/main/AMP-Practice-A-Keplers-Law).

## Story 

NASA's [MMS Spacecraft fleet](https://mms.gsfc.nasa.gov/) experienced an anomaly in their orbit determination algorithms that caused an infinite loop. The software team spent days searching for the culprit and quickly found that the convergence tolerance was set to machine epsilon!

[Read more](story.md) about the the NASA mission that inspired this challenge problem.

![NASA Image](MMSinSpace_small.jpg)

### Background - Kepler's Law

Keplers law is 

`M = E - e sin(E)`

where `M` is the mean anomaly, `E` is the eccentric anomaly, and `e` is the eccentricity. `E` and `M` are in units of radians.

https://en.wikipedia.org/wiki/Kepler%27s_equation

To solve this equation, we have to use a numerical method and iterations. Essentially, we start from an initial guess, and refine it with each iteration until the error converges to 0. Typically this convergence is fast and only takes 3-6 iterations. **However, if the tolerance is too tight, and we are using discrete representation of real numbers (i.e. floating point math), then sometimes the convergence criterion can be missed.** This results in an infinite loop in the numerical method.


## Ingredients

### Inputs and Outputs
The data needed for Kepler's Law formulation as inputs is the mean anomaly, M, and eccentricity, e. Since we are looking at elliptic orbits, the eccentricity is bounded between 0 and 1. M is passed in units of degrees. 

These will be passed in as strings of floating point numbers via the command line:

```bash
./kepler <eccentricity> <mean_anomaly>

```

The output will be the Eccentric Anomaly, E, in degrees. This will be printed as the last line of standard output. The program also logs some diagnostic information, such as confirming the parsed inputs and the error level at each iteration of the numerical solution.

### Example usage:

```bash
./build/kepler 0.3375 36.0
Read eccentricity: 0.337500; mean_anomaly: 36.000000
Tolerance: 1.1920928955e-07
Using Newton-Rahpson to calculate eccentric anomaly based on Kepler's Law
Inputs to determine eccentric anomaly:
        eccentricity (e)= 0.337500 radians
        mean anomaly (M) = 0.628319 radians
        initial guess = 0.628319 radians
Error: -0.198378
Error: 0.00826192
Error: 1.44243e-05
Error: 0
Solution converges to 0.890737 radians with an error of 0.0000000000e+00, which in degrees is:
51.035454


```

### Testing
The pytest framework runs the test scripts, which perform some very simple tests:

- First, that the binary can be executed.
- Second, that we can pass arguments to the binary and get an output.
- Third, that some normal input results in a calculation that completes.
- Fourth, that some "problematic" input does not create an infinite loop. This fails for the original binary!

The tests can be run with:

```
python3 -m pytest test_keplers_law.py
```

By default the binary at `code/build/kepler` will be tested. You can test a different binary using the `--binary` option:

```
python3 -m pytest test_keplers_law.py --binary /path/to/different/binary
```


## Patching the Binary

There are three ways to patch the binary:
1. change the tolerance to be looser with the exit criteria (i.e. patch the data defining the convergence tolerance).
2. add an iteration counter and exit after there are too many iterations (i.e. N_max = 50).
3. add a loop timer and exit after a certain amount of time has passed (i.e. T_max = 250usec).


The first patch candidate is the simplest and would be a minimal change in the binary. However, certain input conditions may still lead to an infinite loop. How can we be sure the patch will be robust?

The second patch will require the addition of a counter and exit criteria check in the loop. This will add additional instructions and variables to the binary. 

The third patch is similar to the second because additional variables and code are needed. 

There may be other ways to fix the problem. If you think of an interesting alternative approach, go ahead and try it!


### Patching the Binary with OFRAK

We strongly recommend using [OFRAK](https://ofrak.com/) to patch the binary. OFRAK is a binary analysis and modification platform that combines the ability to unpack, analyze, modify, and repack binaries; It was built to handle problems like this.

In particular we recommend using the [PatchMaker](https://ofrak.com/docs/user-guide/patch-maker/user-guide.html) subsystem of OFRAK. There are some [OFRAK components](https://ofrak.com/docs/reference/ofrak/core/patch_maker/modifiers.html) which make working with PatchMaker fairly straightforward. However, you can also use the underlying machinery of PatchMaker to patch the binary with more fine-grained control.

Good luck. Red Balloon Security's Interstellar division is counting on you!


### Challenge Logistics

Email us the scripts for your binary patching solutions at safespacectf@redballoonsecurity.com by 3pm Saturday 8/13. Once we pick the winners on Saturday, we'll let them know by email when to expect their prizes.

Oh snap, the prizes! For each of the top three submissions that we think are the "best" solutions, we'll mail a built-out [Antenny board](https://github.com/RedBalloonShenanigans/antenny), plus our usual healthy helping of swag.


![NASA Image](https://raw.githubusercontent.com/RedBalloonShenanigans/antenny/master/doc_images/Platform_setup_2.jpg)
