#include <iostream>
#include <iomanip>
#include <fstream>
#include <boost/program_options.hpp>

using namespace std;

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
double cellmass_adsorber = 0;
double sugar_adsorber = 0;
double butanol_adsorber = 0;
double ethanol_adsorber = 0;


double circulation_ratio;
double dilution_rate;

// current rates of change in fermenter
double cellmass_rate_r;
double sugar_rate_r;
double butanol_rate_r;
double ethanol_rate_r;

std::ofstream flog;

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
    ("circulation_flow_rate ", po::value<double>()->default_value( circulation_flow_rate ), "flow rate from fermenter to adsorber L/h")
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
    dilution_rate      =  feed_flow_rate / volume_fermenter;

}
/** Calulate current rates of change.

Implements mass balance equations 19 to 22
*/
void delta()
{
    // Eq 19
    cellmass_rate_r =
        ( specific_cell_growth_rate - dilution_rate ) * cellmass_fermenter +
        ( cellmass_adsorber - cellmass_fermenter ) * circulation_ratio;

    // Eq 20
    sugar_rate_r =
        ( sugar_feed - sugar_fermenter ) * dilution_rate +
        ( sugar_adsorber - sugar_fermenter ) * circulation_ratio  +
        specific_sugar_growth_rate *  cellmass_fermenter;

    // Eq 21
    butanol_rate_r =
        ( butanol_adsorber - butanol_fermenter) * circulation_ratio  +
        specific_butanol_growth_rate * cellmass_fermenter +
        butanol_fermenter * dilution_rate;

    // Eq 22
    ethanol_rate_r =
        ( ethanol_adsorber - ethanol_fermenter) *circulation_ratio  +
        specific_ethanol_growth_rate * cellmass_fermenter +
        ethanol_fermenter * dilution_rate;
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
         <<  std::setw(10) << butanol_fermenter << endl;

}

int main( int ac, char* av[] )
{
    // read staarting parameters
    read_params( ac, av );

    // open log file
    flog.open("test.txt");
    flog << "cellmass     sugar      ethanol     butanol\n";

    // loop over simulation timesteps
    for( int t = 0; t < 10; t++ )
    {
        // write current values to log file
        dump();

        // current rates of change
        delta();

        // calculate new values after simulation timestep
        increment( 0.0625 );


    }

    return 0;
}
