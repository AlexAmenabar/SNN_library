Configuration file parameters:
- Execution type: clock-based (0) or event-driven (1).
- Neuron type: LIF neuron model (0)
- Run learning rules: No (0) / Yes (1)
- SNN file: directory
- Number of processes to run the simulation: n > 0 (compile with openmp flag!)
- Store the SNN after simulation: No (0) / Yes (1)
    - If yes, file name and directory to store it

- Directory/file name to store generated spike times
- Directory/file name to store final weights
- Directory/file name to store execution times
- Directory/file name to store spikes generated per each neuron

- If the input is a data stream
    - Directory/file name to load them
    - Time steps to simulate

- If the input is divided into samples
    - TODO


Spiking neural network file format:
- Number of neurons
- Number of input neurons
- Number of output neurons
- Number of synapses
- Number of input synapses
- Number of output synapses

