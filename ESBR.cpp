#include <iostream>
#include <iomanip>
#include <fstream>
#include <boost/program_options.hpp>

using namespace std;

std::ofstream flog;

/*  Units: unless specified otherwise, grams, litres and hours */

/* Values from table 1 */
double cellmass_stops_growth = 9.806;
double cellmass_inhibition_constant = 0.4583;
double butanol_stops_growth = 12.57;
double butanol_inhibition_constant = 0.0159;

/* estimated values of μm, KS, and KI for the growth of
the recombinant Clostridium acetobutylicum under substratelimiting
conditions are 0.238 h−1, 0.357 g/L, and 272.3 g/L,
respectively.
*/

double cell_growth_rate_max = 0.238;
double substrate_saturation_constant = 0.357;
double substrate_inhibition_constant = 272.3;

double volume_fermenter = 10;
double feed_flow_rate = 0.0;
double sugar_feed = 1.0;
double circulation_flow_rate = 0 ;
double specific_cell_growth_rate = 0.0625;
double specific_sugar_growth_rate = 0.0625;
double specific_butanol_growth_rate = 0.015625;
double specific_ethanol_growth_rate = 0.0625;
double cellmass_fermenter = 100;
double sugar_fermenter = 1.0;
double butanol_fermenter = 0;
double ethanol_fermenter = 0;


double circulation_ratio;
double dilution_rate;

// current rates of change in fermenter
double cellmass_rate_r;
double sugar_rate_r;
double butanol_rate_r;
double ethanol_rate_r;

double butanol_adsorption_rate;
double ethanol_adsorption_rate;

class cFermenter
{
public:
    double cell_growth_rate;

    void Delta();
};

class cAdsorber
{
public:
    double mass_kg = 10;
    double volume = 1000;
    double butanol_kinetic_min = 10;
    double ethanol_kinetic_min = 10;
    double butanol_equilibrium = 1;
    double ethanol_equilibrium = 1;
    double ethanol = 0;
    double butanol = 0;
    double cellmass_rate;
    double sugar_rate;
    double butanol_rate;
    double ethanol_rate;
    double cellmass = 0;
    double sugar = 0;
    double cell_net_growth = 0.0625;
    double circulation_ratio;

    void Increment( double time_step )
    {
        cellmass += cellmass_rate * time_step;
        sugar    += sugar_rate * time_step;
        butanol  += butanol_rate * time_step;
        ethanol  += ethanol_rate * time_step;
    }
    /** Write adsorption rates to log file

    The delta function has given us the rate of change in component concentration

    We want to display the total grams of component adsorbed each hour
    so we need to reverse the sign and multiply by the volume

     */
    void dump()
    {
        flog << std::setprecision(3);
        flog << std::setw(12) << - butanol_rate * volume
             <<  std::setw(12) << - ethanol_rate * volume;

    }
};

cFermenter fermenter;
cAdsorber adsorber;


void cFermenter::Delta()
{
    // Eq 1
    double butane_inhibition_factor =
        pow( 1 - butanol_fermenter / butanol_stops_growth, butanol_inhibition_constant );
    double cellmass_inhibition_factor =
        pow( 1 - cellmass_fermenter / cellmass_stops_growth, cellmass_inhibition_constant );
    cell_growth_rate =
        cell_growth_rate_max * sugar_fermenter * butane_inhibition_factor * cellmass_inhibition_factor /
        ( substrate_saturation_constant + sugar_fermenter + sugar_fermenter * sugar_fermenter / substrate_inhibition_constant );
}


void read_params( int ac, char* av[] )
{

    namespace po = boost::program_options;

    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "produce help message")
    ("volume_fermenter", po::value<double>()->default_value( 10 ), "fermenter volume L")
    ("feed_flow_rate ", po::value<double>()->default_value( feed_flow_rate ), "feed flow rate L/h")
    ("sugar_feed", po::value<double>()->default_value(sugar_feed),"sugar concentration in feed g/L")
    ("circulation_flow_rate", po::value<double>()->default_value( circulation_flow_rate ), "flow rate from fermenter to adsorber L/h")
    ("cell_growth_rate",  po::value<double>()->default_value( 0.0625 ), "specific cell growth rate g/g/h ")
    ("sugar_growth_rate", po::value<double>()->default_value( 0.0625 ), "specific sugar growth rate g/g/h ")
    ("butanol_growth_rate", po::value<double>()->default_value( specific_butanol_growth_rate ), "specific butanol growth rate g/g/h ")
    ("ethanol_growth_rate", po::value<double>()->default_value( 0.0625 ), "specific ethanol growth rate g/g/h ")
    ("cellmass_fermenter", po::value<double>()->default_value( cellmass_fermenter ), "starting cell mass in fermenter g")
    ("sugar_fermenter",  po::value<double>()->default_value( sugar_fermenter ), "starting sugar in fermenter g/L" )
    ;

    // parse the command line
    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);

    // parse the config file
    std::ifstream ifs("parameters.txt");
    if (ifs)
    {
        store(parse_config_file(ifs, desc), vm);
        notify(vm);
    }

    // extract the parameter values
    if (vm.count("help"))
    {
        cout << desc << "\n";
        exit(1);
    }

    // set the values
    if( vm.count("volume_fermenter"))
    {
        volume_fermenter = vm["volume_fermenter"].as<double>();
    }
    if( vm.count("feed_flow_rate"))
    {
        feed_flow_rate = vm["feed_flow_rate"].as<double>();
    }
    if( vm.count("sugar_feed"))
    {
        sugar_feed = vm["sugar_feed"].as<double>();
    }
    if( vm.count("circulation_flow_rate"))
    {
        circulation_flow_rate = vm["circulation_flow_rate"].as<double>();
    }
    if( vm.count("cell_growth_rate"))
    {
        specific_cell_growth_rate = vm["cell_growth_rate"].as<double>();
    }
    if( vm.count("sugar_growth_rate"))
    {
        specific_sugar_growth_rate = vm["sugar_growth_rate"].as<double>();
    }
    if( vm.count("butanol_growth_rate"))
    {
        specific_butanol_growth_rate = vm["butanol_growth_rate"].as<double>();
    }
    if( vm.count("ethanol_growth_rate"))
    {
        specific_ethanol_growth_rate = vm["ethanol_growth_rate"].as<double>();
    }
    if( vm.count("cellmass_fermenter"))
    {
        cellmass_fermenter = vm["cellmass_fermenter"].as<double>();
    }
    if( vm.count("sugar_fermenter"))
    {
        sugar_fermenter = vm["sugar_fermenter"].as<double>();
    }

    circulation_ratio =  circulation_flow_rate / volume_fermenter;
    adsorber.circulation_ratio = circulation_flow_rate / adsorber.volume;
    dilution_rate      =  feed_flow_rate / volume_fermenter;

}
/** Calulate current rates of change.

Implements mass balance equations
*/
void delta()
{
    // fermenter



    // Eq 19
    cellmass_rate_r =
        ( specific_cell_growth_rate - dilution_rate ) * cellmass_fermenter +
        ( adsorber.cellmass - cellmass_fermenter ) * circulation_ratio;

    // Eq 20
    sugar_rate_r =
        ( sugar_feed - sugar_fermenter ) * dilution_rate +
        ( adsorber.sugar - sugar_fermenter ) * circulation_ratio  +
        specific_sugar_growth_rate *  cellmass_fermenter;

    // Eq 21
    butanol_rate_r =
        ( adsorber.butanol - butanol_fermenter) * circulation_ratio  +
        specific_butanol_growth_rate * cellmass_fermenter +
        butanol_fermenter * dilution_rate;

    // Eq 22
    ethanol_rate_r =
        ( adsorber.ethanol - ethanol_fermenter) *circulation_ratio  +
        specific_ethanol_growth_rate * cellmass_fermenter +
        ethanol_fermenter * dilution_rate;

    // adsorber

    // Eq 13
    butanol_adsorption_rate =
        - ( adsorber.mass_kg / adsorber.volume ) * adsorber.butanol_kinetic_min *
        ( adsorber.butanol_equilibrium - adsorber.butanol / adsorber.mass_kg  );
    ethanol_adsorption_rate =
        - ( adsorber.mass_kg / adsorber.volume ) * adsorber.ethanol_kinetic_min *
        ( adsorber.ethanol_equilibrium - adsorber.ethanol / adsorber.mass_kg  );

    // Eq 23
    adsorber.cellmass_rate =
        ( adsorber.cell_net_growth * adsorber.cellmass ) +
        ( cellmass_fermenter - adsorber.cellmass ) * adsorber.circulation_ratio;

    // Eq 24
    adsorber.sugar_rate =
        ( sugar_fermenter - adsorber.sugar ) * adsorber.circulation_ratio +
        specific_sugar_growth_rate * adsorber.cellmass;

    // Eq 25
    adsorber.butanol_rate =
        ( butanol_fermenter - adsorber.butanol ) * adsorber.circulation_ratio +
        specific_butanol_growth_rate * adsorber.cellmass +
        butanol_adsorption_rate;

    // Eq 26
    adsorber.ethanol_rate =
        ( ethanol_fermenter - adsorber.ethanol ) * adsorber.circulation_ratio +
        specific_ethanol_growth_rate * adsorber.cellmass +
        ethanol_adsorption_rate;

}

void increment( double time_step )
{
    cellmass_fermenter += time_step * cellmass_rate_r;
    sugar_fermenter    += time_step * sugar_rate_r;
    butanol_fermenter  += time_step * butanol_rate_r;
    ethanol_fermenter  += time_step * ethanol_rate_r;


}

void dump()
{
    flog << std::setprecision(3);
    flog << std::setw(10) << cellmass_fermenter
         <<  std::setw(10) << sugar_fermenter
         <<  std::setw(10) << ethanol_fermenter
         <<  std::setw(10) << butanol_fermenter;

}

int main( int ac, char* av[] )
{
    // read staarting parameters
    read_params( ac, av );

    // open log file
    flog.open("test.txt");
    flog << "             fermenter                               adsorber\n";
    flog << "cellmass     sugar      ethanol  butanol    ethanol g/h   butanol g/h\n";

    // loop over simulation timesteps
    for( int t = 0; t < 20; t++ )
    {
        // write current values to log file
        dump();
        flog << " || ";
        adsorber.dump();
        flog << endl;

        // current rates of change
        delta();

        // calculate new values after simulation timestep
        increment( 0.0625 );
        adsorber.Increment( 0.0625 );


    }

    return 0;
}
