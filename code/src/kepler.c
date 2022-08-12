#include <stdio.h>
#include "kepler.h"


float tol = __FLT_EPSILON__;

void usage(char * program) {
    fprintf(stderr,"Usage %s <eccentricity> <mean_anomaly>\nPrints calculated mean_anomaly\n", program);
}

/**
  * @brief  On getting a sig to kill program, set exit status to gracefully close program. sig_alrm is used for
  *         handling address claim timeouts
  *
  * @param  sig:  Signal given
  * @param  info: Additional signal info
  * @param  vp:   signal context structure
  */
static void sighandler(int sig, siginfo_t *info, void *vp) {
    switch (sig) {
        case SIGINT:
            exit(1);
            break;
        case SIGTERM:  // gracefully exit (close socket)
            exit(1);
            break;
        case SIGALRM:
            break;
    }
}


/**
  * @brief  Installs signals with the sighandler function
  *
  * @param  sig:  Signal to install
  */
static void install_signal(int sig) {
    int ret;
    struct sigaction sigact = {
            .sa_sigaction = sighandler,
            .sa_flags = SA_SIGINFO,
    };

    sigfillset(&sigact.sa_mask);
    ret = sigaction(sig, &sigact, NULL);
    if (ret < 0)
        err(1, "sigaction for signal %i", sig);
}



int Keplers_Law(
        float mean_anomaly,
        float eccentricity,
        float initial_guess,
        float * eccentric_anomaly_output,
        //int * iterations_output,
        float * error_output){
    printf("Using Newton-Rahpson to calculate eccentric anomaly based on Kepler's Law\n");
    printf("Inputs to determine eccentric anomaly:\n\teccentricity (e)= %0.6f radians\n\tmean anomaly (M) = %0.6f radians\n\tinitial guess = %0.6f radians\n", eccentricity,mean_anomaly,initial_guess);
    float error;
    float E;
    // Add this iteration counter as part of the patch
    // int n;
    // n = 0;
    // int convergence_flag;
    // convergence_flag = 1;
    E = initial_guess;
    error = 1;
    while (fabsf(error) > tol){
        error = formula(E, eccentricity, mean_anomaly);
        printf("Error: %g\n", error);
        E = E - error/formula_derivative(E,eccentricity);
        // Add this loop protection as part of the patch
        // n++;
        // if (n >= MAX_ITERATIONS){
        //      convergence_flag = 0;    
        //      break;
        // }
    }
    *eccentric_anomaly_output = E;
    // *iterations_output = n + 1; // n indexes from zero, so we need to add 1 to get interations
    *error_output = error;
    // replace this with return convergence flag for the patch.
    return 1; 
}


float formula(float ecc_anomaly, float eccentricity, float mean_anomaly)
{
    return (ecc_anomaly - eccentricity*(sinf(ecc_anomaly)) - mean_anomaly);
}
float formula_derivative(float ecc_anomaly, float eccentricity)
{
    return (1-eccentricity*(cosf(ecc_anomaly)));
}

void read_input(char** argv, float* eccent, float* mean_anomaly){
    char* raw_eccent_str = argv[1];
    char* raw_mean_anomaly_str = argv[2];
    *eccent = (float) strtof(raw_eccent_str, NULL);
    *mean_anomaly = (float) strtof(raw_mean_anomaly_str, NULL);
}


int main(int argc, char **argv) {
    if (argc < 3){
        usage(argv[0]); 
        exit(1);
    }
    // Install signals
    install_signal(SIGTERM);
    install_signal(SIGINT);
    install_signal(SIGALRM);

    int ret;
    float eccentricity;
    float mean_anomaly;
    struct space_data inputs;
    uint32_t out_mean_anomaly_int;

    // We will use the mean anomaly as our initial guess
    read_input(argv, &eccentricity, &mean_anomaly);

    printf("Read eccentricity: %f; mean_anomaly: %f\n", eccentricity, mean_anomaly);
    printf("Tolerance: %.10e\n", tol);

    if (eccentricity >= 1) eccentricity = 0.9999999;

    float anom_deg = mean_anomaly;
    //TODO: check to see if data is in bounds
    // ensure the mean anomaly is between 0 and 2*pi radians
    while (anom_deg > 360) anom_deg -= 360;
    while (anom_deg < 0) anom_deg += 360;
    mean_anomaly = pi*anom_deg/180.0;
    // int out_iterations; //
    float out_mean_anomaly = 0;
    float out_error = 0;
    ret = Keplers_Law(mean_anomaly,
                      eccentricity,
                      mean_anomaly,
                      &out_mean_anomaly,
                      &out_error);
    // Convert out_mean_anomaly from radians into the J1939 SLOT
    printf("Solution converges to %f radians with an error of %.10e, which in degrees is:\n", out_mean_anomaly, out_error);
    float deg = 180*out_mean_anomaly/pi;
    while (deg > 180) deg -= 360;
    while (deg < -180) deg += 360;
    printf("%f\n", deg);
}
