#include <iostream>
#include <iomanip>
#include <fstream>
#include <boost/program_options.hpp>

using namespace std;

std::ofstream flog;

/*  Units: unless specified otherwise, grams, litres and hours */



double volume_fermenter = 10;
double feed_flow_rate = 0.0;
double sugar_feed = 1.0;
double circulation_flow_rate = 0 ;
//double specific_cell_growth_rate = 0.0625;
double specific_sugar_growth_rate = 0.0625;
double specific_butanol_growth_rate = 0.015625;
double specific_ethanol_growth_rate = 0.0625;


double circulation_ratio;
double dilution_rate;

double butanol_adsorption_rate;
double ethanol_adsorption_rate;

class cYeast
{
public:
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

    /** Cell growth rate g/h

    @param[in] sugar g/l
    @param[in] butanol g/l
    @param[in] cellmass g/l

    Implements equation 1

    */
    double Delta(
        double sugar,
        double butanol,
        double cellmass
    );

};

class cFermenter
{
public:

    // component concentrations g/l
    double cellmass = 1;
    double sugar    = 1.0;
    double butanol  = 0;
    double ethanol  = 0;

    // component rates of change /h
    double cell_specific_rate;
    double cell_growth_rate;
    double sugar_rate;
    double butanol_rate;
    double ethanol_rate;

    void Delta();
    void Increment( double time_step );
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
    double cell_specific_rate;
    double cell_growth_rate;
    double sugar_rate;
    double butanol_rate;
    double ethanol_rate;
    double cellmass = 0;
    double sugar = 0;
    double circulation_ratio;

    void Increment( double time_step )
    {
        cellmass += cell_growth_rate * time_step;
        sugar    += sugar_rate * time_step;
        butanol  += butanol_rate * time_step;
        ethanol  += ethanol_rate * time_step;
    }

    void Delta();

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
cYeast yeast;


void cFermenter::Delta()
{
    cell_specific_rate = yeast.Delta(
                             sugar,
                             butanol,
                             cellmass );
}
void cAdsorber::Delta()
{
    cell_specific_rate = yeast.Delta(
                             sugar,
                             butanol,
                             cellmass );
}
double cYeast::Delta(
    double sugar,
    double butanol,
    double cellmass
)
{

    // Eq 1
    double butane_inhibition_factor =
        pow( 1 - butanol / butanol_stops_growth, butanol_inhibition_constant );

    double cellmass_inhibition_factor =
        pow( 1 - cellmass / cellmass_stops_growth, cellmass_inhibition_constant );

    double substrate_inhibition_factor =
        1 / ( substrate_saturation_constant + sugar + sugar * sugar / substrate_inhibition_constant );

    double r =
        cell_growth_rate_max * sugar * butane_inhibition_factor * cellmass_inhibition_factor * substrate_inhibition_factor;

    return r;
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
    ("sugar_growth_rate", po::value<double>()->default_value( 0.0625 ), "specific sugar growth rate g/g/h ")
    ("butanol_growth_rate", po::value<double>()->default_value( specific_butanol_growth_rate ), "specific butanol growth rate g/g/h ")
    ("ethanol_growth_rate", po::value<double>()->default_value( 0.0625 ), "specific ethanol growth rate g/g/h ")
    ("cellmass_fermenter", po::value<double>()->default_value( fermenter.cellmass ), "starting cell mass in fermenter g/L")
    ("sugar_fermenter",  po::value<double>()->default_value( fermenter.sugar ), "starting sugar in fermenter g/L" )
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
        fermenter.cellmass = vm["cellmass_fermenter"].as<double>();
    }
    if( vm.count("sugar_fermenter"))
    {
        fermenter.sugar = vm["sugar_fermenter"].as<double>();
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

    fermenter.Delta();

    // Eq 19
    fermenter.cell_growth_rate =
        ( fermenter.cell_specific_rate - dilution_rate ) * fermenter.cellmass +
        ( adsorber.cellmass - fermenter.cellmass ) * circulation_ratio;

    // Eq 20
    fermenter.sugar_rate =
        ( sugar_feed - fermenter.sugar ) * dilution_rate +
        ( adsorber.sugar - fermenter.sugar ) * circulation_ratio  +
        specific_sugar_growth_rate *  fermenter.cellmass;

    // Eq 21
    fermenter.butanol_rate =
        ( adsorber.butanol - fermenter.butanol ) * circulation_ratio  +
        specific_butanol_growth_rate * fermenter.cellmass +
        fermenter.butanol * dilution_rate;

    // Eq 22
    fermenter.ethanol_rate =
        ( adsorber.ethanol - fermenter.ethanol) *circulation_ratio  +
        specific_ethanol_growth_rate * fermenter.cellmass +
        fermenter.ethanol * dilution_rate;

    // adsorber

    adsorber.Delta();

    // Eq 13
    butanol_adsorption_rate =
        - ( adsorber.mass_kg / adsorber.volume ) * adsorber.butanol_kinetic_min *
        ( adsorber.butanol_equilibrium - adsorber.butanol / adsorber.mass_kg  );
    ethanol_adsorption_rate =
        - ( adsorber.mass_kg / adsorber.volume ) * adsorber.ethanol_kinetic_min *
        ( adsorber.ethanol_equilibrium - adsorber.ethanol / adsorber.mass_kg  );

    // Eq 23
    adsorber.cell_growth_rate =
        ( adsorber.cell_specific_rate * adsorber.cellmass ) +
        ( fermenter.cellmass - adsorber.cellmass ) * adsorber.circulation_ratio;

    // Eq 24
    adsorber.sugar_rate =
        ( fermenter.sugar - adsorber.sugar ) * adsorber.circulation_ratio +
        specific_sugar_growth_rate * adsorber.cellmass;

    // Eq 25
    adsorber.butanol_rate =
        ( fermenter.butanol - adsorber.butanol ) * adsorber.circulation_ratio +
        specific_butanol_growth_rate * adsorber.cellmass +
        butanol_adsorption_rate;

    // Eq 26
    adsorber.ethanol_rate =
        ( fermenter.ethanol - adsorber.ethanol ) * adsorber.circulation_ratio +
        specific_ethanol_growth_rate * adsorber.cellmass +
        ethanol_adsorption_rate;

}

void cFermenter::Increment( double time_step )
{
    fermenter.cellmass += time_step * fermenter.cell_growth_rate;
    fermenter.sugar    += time_step * fermenter.sugar_rate;
    fermenter.butanol  += time_step * fermenter.butanol_rate;
    fermenter.ethanol  += time_step * fermenter.ethanol_rate;


}

void dump()
{
    flog << std::setprecision(3);
    flog << std::setw(10) << fermenter.cellmass
         <<  std::setw(10) << fermenter.sugar
         <<  std::setw(10) << fermenter.ethanol
         <<  std::setw(10) << fermenter.butanol;

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
        fermenter.Increment( 0.0625 );
        adsorber.Increment( 0.0625 );


    }

    return 0;
}
