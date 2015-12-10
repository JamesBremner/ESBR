# ESBR
Simulation of a Fermentation Process with Ex situ Butanol Recovery (ESBR) for Continuous Biobutanol Production

The simulator is based on this paper  http://pubs.acs.org/doi/pdf/10.1021/acs.energyfuels.5b01031

Parameters can be entered from the command line ( or from a parameters.txt file )
<pre>
ESBR.exe --help
Allowed options:
  --help                                produce help message
  --volume_fermenter arg (=10)          fermenter volume L
  --feed_flow_rate  arg (=0)            feed flow rate L/h
  --sugar_feed arg (=1)                 sugar concentration in feed g/L
  --circulation_flow_rate  arg (=0)     flow rate from fermenter to adsorber
                                        L/h
  --cell_growth_rate arg (=0.0625)      specific cell growth rate g/g/h
  --sugar_growth_rate arg (=0.0625)     specific sugar growth rate g/g/h
  --butanol_growth_rate arg (=0.015625) specific butanol growth rate g/g/h
  --ethanol_growth_rate arg (=0.0625)   specific ethanol growth rate g/g/h
  --cellmass_fermenter arg (=100)       starting cell mass in fermenter g
  --sugar_fermenter arg (=1)            starting sugar in fermenter g/L
  </pre>

The output looks like this

<pre>
             fermenter                               adsorber
cellmass     sugar      ethanol  butanol    ethanol g/h   butanol g/h
       100         1         0         0 ||           -0          -0
      99.8      1.38     0.391    0.0977 ||          100         100
      99.5      1.77     0.778     0.194 ||         99.9        99.3
      99.3      2.14      1.16      0.29 ||         99.7        98.6
      99.1      2.52      1.54     0.385 ||         99.6        97.8
      98.8      2.89      1.92      0.48 ||         99.4        97.1
      98.6      3.26      2.29     0.573 ||         99.3        96.4
      98.4      3.62      2.66     0.665 ||         99.2        95.7
      98.1      3.98      3.03     0.757 ||           99          95
      97.9      4.34       3.4     0.848 ||         98.9        94.3
      97.7       4.7      3.76     0.938 ||         98.8        93.6
      97.5      5.05      4.11      1.03 ||         98.6        92.9
      97.2       5.4      4.47      1.12 ||         98.5        92.2
        97      5.74      4.82       1.2 ||         98.4        91.5
      96.8      6.09      5.17      1.29 ||         98.2        90.8
      96.5      6.43      5.51      1.38 ||         98.1        90.1
      96.3      6.76      5.86      1.46 ||           98        89.4
      96.1       7.1      6.19      1.54 ||         97.8        88.7
      95.9      7.43      6.53      1.63 ||         97.7          88
      95.6      7.76      6.86      1.71 ||         97.6        87.3
</pre>
