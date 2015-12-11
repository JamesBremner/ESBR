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
             fermenter                   ||      adsorber
cellmass     sugar       ethanol  butanol||  ethanol g/h   butanol g/h
         1       200         0         0 ||           -0          -0
      1.01       200   0.00391  0.000977 ||            2           2
      1.02       200   0.00782   0.00193 ||         2.02        2.02
      1.02       200    0.0117   0.00287 ||         2.05        2.05
      1.03       200    0.0156   0.00379 ||         2.08        2.08
      1.04       200    0.0196    0.0047 ||          2.1         2.1
      1.05       200    0.0235   0.00558 ||         2.13        2.13
      1.06       200    0.0274   0.00644 ||         2.15        2.15
      1.07       200    0.0314   0.00728 ||         2.18        2.18
      1.07       200    0.0353    0.0081 ||         2.21        2.21
      1.08       200    0.0392   0.00891 ||         2.24        2.24
      1.09       200    0.0432   0.00969 ||         2.26        2.26
       1.1       200    0.0471    0.0104 ||         2.29        2.29
      1.11       200    0.0511    0.0112 ||         2.32        2.32
      1.12       199     0.055    0.0119 ||         2.35        2.35
      1.13       199     0.059    0.0126 ||         2.38        2.38
      1.13       199     0.063    0.0133 ||         2.41        2.41
      1.14       199    0.0669    0.0139 ||         2.44        2.44
      1.15       199    0.0709    0.0146 ||         2.47        2.47
      1.16       199    0.0749    0.0152 ||          2.5         2.5
      1.17       199    0.0788    0.0157 ||         2.53        2.53
      1.18       199    0.0828    0.0163 ||         2.56        2.56
      1.19       199    0.0868    0.0168 ||         2.59        2.59
       1.2       199    0.0908    0.0174 ||         2.62        2.62

</pre>
