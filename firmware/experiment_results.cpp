#include<experiment_results.h>
#include<Experiment.h>

void notify_experiment_results(Experiment experiment) {
    int offset = 0;

    for (int i = 0; i < SAMPLES_PER_PACKET; i++) {
        DataPoint dp;

        if (currentExperiment->results_buffer.pop(dp)) { // pop from ring buffer into dp
            memcpy(&packet[offset], &dp, BYTES_PER_SAMPLE); // DataPoint is {uint32_t, int32_t, float} which is 12 bytes packed and little endian so I think we can copy it
            offset += BYTES_PER_SAMPLE;
        }
    }
}