#ifndef starsky__gnutellafilespacenetwork
#define starsky__gnutellafilespacenetwork

#include "gnutellacontent.h"
#include "gnutellanode.h"
#include "gnutellaquery.h"

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include "superstring.h"
#include "npartitenetwork.h"

namespace Starsky {

/**
 * This class models the structure of content and nodes in Gnutella.
 * The data is obtained from listening to query hits on gnutella networks.
 * These hits have a servent id (a unique string for each servent), and file
 * information (such as the hash, name, and size of the file).  The goal is
 * to examine any structure one might see in the bipartite network that one
 * sees when nodes are connected to content they give hits for.
 */
	
class GnutellaFileSpaceNetwork : public NPartiteNetwork {

    public:
	/**
	 * Read in a network from a gnutella trace data file
	 * @param infile a stream which has the gnutella trace data
	 * @param max the maximum number of nodes to create.  The
	 * data may be much larger than we can handle in memory
	 */
        GnutellaFileSpaceNetwork(std::istream& infile, int max=300000);

	/**
	 * Weight of a content node is the number of nodes have the file
	 * divided by the number of queries which match it.  It is the
	 * number of observed nodes having the file, per query.
	 * For each weight, the map tells us how many occurances there were
	 */
        std::map<double,int> getContentWeights() const; 
	    /**
	     * holds the integer to represent content in the NPartiteNet
	     */
	    static const int content_type;
	    /**
	     * holds the integer to represent nodes in the NPartiteNet
	     */
	    static const int node_type;
	    /**
	     * holds the integer to represent queries in the NPartiteNet
	     */
	    static const int query_type;
	
    protected:
	/**
	 * The constructor calls this one.  It is to keep the constructor
	 * clean.
	 * @param infile the stream to read one query hit from
	 * @param mid the Message ID for this query
	 * @return the number of new nodes created.
	 */
	int readQuery(std::istream& infile, const std::string& mid);
	/**
	 * @param infile the stream to read one query hit from
	 * @param sid the ServentID for this queryhit
	 * @return the number of new nodes created.
	 */
        int readQueryHit(std::istream& infile, const std::string& sid);
	
	std::map<std::string, GnutellaNode*> _node_by_name;
	std::map<std::string, GnutellaContent*> _content_by_hash;
        std::map<std::string, GnutellaQuery*> _query_by_mid;	
};
	
}

#endif
