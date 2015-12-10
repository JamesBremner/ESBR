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
