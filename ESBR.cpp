#include <iostream>
#include <iomanip>
#include <fstream>
#include <boost/program_options.hpp>

using namespace std;

double volume_fermenter = 10;
double feed_flow_rate = 0.0;
double circulation_flow_rate = 0 ;
double specific_cell_growth_rate = 0.0625;
double cellmass_fermenter = 100;
double cellmass_adsorber = 0;


double circulation_ratio;
double dilution_rate;

double cellmass_rate_r;

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
    ("circulation_flow_rate ", po::value<double>()->default_value( circulation_flow_rate ), "flow rate from fermenter to adsorber L/h")
    ("cell_growth_rate",  po::value<double>()->default_value( 0.0625 ), "specific cell growth rate, g/g/h ")
    ("cellmass_fermenter", po::value<double>()->default_value( cellmass_fermenter ), "starting cell mass in fermenter g")
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
    if( vm.count("circulation_flow_rate"))
    {
        circulation_flow_rate = vm["circulation_flow_rate"].as<double>();
    }
    if( vm.count("specific_cell_growth_rate"))
    {
        specific_cell_growth_rate = vm["specific_cell_growth_rate"].as<double>();
    }
    if( vm.count("cellmass_fermenter"))
    {
        cellmass_fermenter = vm["cellmass_fermenter"].as<double>();
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
}

void increment( double time_step )
{
    cellmass_fermenter += time_step * cellmass_rate_r;
}

void dump()
{

    flog << std::setw(10) << cellmass_fermenter
         <<  std::setw(10) <<cellmass_rate_r << endl;

}

int main( int ac, char* av[] )
{
    // read staarting parameters
    read_params( ac, av );

    // open log file
    flog.open("test.txt");

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