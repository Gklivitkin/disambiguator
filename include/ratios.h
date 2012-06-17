
#ifndef PATENT_RATIOS_H
#define PATENT_RATIOS_H

#include <iostream>
#include <map>
#include <set>
#include <fstream>

#include "disambiguation.h"
#include "attribute.h"

using std::string;
using std::set;
using std::map;



/**
 * cMonotonic_Similarity_Compare:
 *
 * For two given similarity profiles, it only compares the
 * scores at a certain position.
 *
 * This class should ONLY serve as a comparison functor
 * for associated containers, i.e., map or set.
 *
 * Private:
 *         unsigned int compare_entry: the position of interest.
 *
 * Public:
 *    bool operator() ( const SimilarityProfile * p1, const SimilarityProfile * p2 ) const:
 *        to compare the two similarity profiles at the position of compare_entry.
 *
 *    cMonotonic_Similarity_Compare( const unsigned int entry): constructor
 *
 *    void reset_entry( const unsigned int entry): reset the variable compare_entry to the input entry.
 */
struct cMonotonic_Similarity_Compare {

private:
    unsigned int compare_entry;
public:
    bool operator() ( const SimilarityProfile * p1, const SimilarityProfile * p2 ) const {
        return p1->at(compare_entry) < p2->at(compare_entry);
    }
    cMonotonic_Similarity_Compare( const unsigned int entry) : compare_entry(entry) {};
    void reset_entry( const unsigned int entry) { compare_entry = entry;}
};


/**
 * MonotonicSet:
 * members in this set is sorted by a given similarity entry in an ascending way.
 */
typedef set< const SimilarityProfile *, cMonotonic_Similarity_Compare> MonotonicSet;


/**
 * cSimilarity_With_Monotonicity_Dimension:
 *
 * This class wraps a similarity profile for a map or
 * set structure. The primary purpose of the class
 * is to allow comparison of similarity profiles
 * skipping a certain entry in the profile.
 *
 * Private:
 *         const SimilarityProfile * psim: pointer to a similarity profile.
 *
 *         unsigned int monotonic_dimension: an entry in which
 *         comparison of similarity profiles will skip.
 *
 *         bool compare_without_primary( const SimilarityProfile * p1, const SimilarityProfile * p2 ) const:
 *             compare the similarity profiles in all dimensions except the "monotonic_dimension" dimension.
 *
 * Public:
 *        bool operator < ( const cSimilarity_With_Monotonicity_Dimension & rhs) const:
 *            comparison function that is used only in map/set.
 *
 *        const unsigned int get_monotonic_dimension() const: return the monotunic_dimension
 *
 *        cSimilarity_With_Monotonicity_Dimension( const SimilarityProfile * p, const unsigned int dm ):
 *                constructor.
 *
 * Use of the above classes is primarily in the DisambigRatioSmoothing.cpp.
 * It is not expected to have a very solid understanding of the above
 * classes, unless smoothing, interpolation and extrapolation of
 * similarity profiles are supposed to change.
 */
struct cSimilarity_With_Monotonicity_Dimension {

private:
    const SimilarityProfile * psim;
    unsigned int monotonic_dimension;
    bool compare_without_primary( const SimilarityProfile * p1, const SimilarityProfile * p2 ) const;
public:
    bool operator < ( const cSimilarity_With_Monotonicity_Dimension & rhs) const;
    const unsigned int get_monotonic_dimension() const {return monotonic_dimension;}
    explicit cSimilarity_With_Monotonicity_Dimension( const SimilarityProfile * p, const unsigned int dm )
        : psim ( p ), monotonic_dimension(dm) {}
};




/**
 * cRatioComponent:
 * This class is used as intermediate steps to finalize a cRatio object.
 * In the current engine, a complete similarity
 * profile is composed of two parts, and each part has several components:
 *
 * Personal: cFirstname, cMiddlename, cLastname
 * Patent: cLatitude, cAssignee, cCoauthor, cClass
 *
 * A cRatioComponent object usually take care of one part of the
 * similarity profiles, and a cRatio object ( will be
 * introduced below ) reads all the necessary cRatioComponent
 * objects to finalize, after which the cRatioComponents are
 * useless.
 */

/*
 * Private:
 *
 *         map < vector <unsigned int>, double, cSimilarity_Compare > ratio_map:
 *             a ratio map for the current component.
 *               Key = similarity profile,
 *               Value = ratio,
 *               Comparator = cSimilarity_Compare
 *
 *         vector < unsigned int > positions_in_ratios:
 *             positions of the current components in the complete similarity profile.
 *
 *         vector < unsigned int > positions_in_record:
 *             position of the current components in the Record::column_names.
 *
 *        const string attrib_group:
 *            the attribute GROUP identifier for which the
 *            cRatioComponent object represents.
 *
 *        const map < string, const Record *> * puid_tree:
 *            the pointer to a map of unique record id string to
 *            its correspoinding record pointer.
 *
 *        map < cSimilarity_With_Monotonicity_Dimension, MonotonicSet > similarity_map:
 *              a map of similarity profiles and their monotonic set for a certain dimension.
 *
 *        vector < string > attrib_names:
 *            attribute names that belong the the atribute group.
 *
*        map < vector < unsigned int > , unsigned int, cSimilarity_Compare > x_counts, m_counts:
 *              maps of similarity profiles to their occurrences in non-match and match training sets.
 */

/**
*
 *  void read_train_pairs ( list < std::pair < string, string > & trainpairs, const char * txt_file ) const:
 *      read the list of pairs of unique record numbers ( training sets ) from the
 *      specified "txt_file" into the list "trainpairs".
 *
 *  void get_similarity_info():
 *      to get the information of similarity profiles of the attribute group.
 *
 */
class cRatioComponent {

    class cException_Partial_SP_Missing : public cAbstract_Exception {
    public:
        cException_Partial_SP_Missing(const char* errmsg): cAbstract_Exception(errmsg){};
    };

private:
   /**  static const unsigned int laplace_base:
    *  a value used for laplacian operations of
    *  obtained similarity profiles to get a ratio.
    */
    static const unsigned int laplace_base;

    map < vector <unsigned int>, double, cSimilarity_Compare > ratio_map;

    vector < unsigned int > positions_in_ratios;

    vector < unsigned int > positions_in_record;

    const string attrib_group;

    const map < string, const Record *> * puid_tree;

    map < cSimilarity_With_Monotonicity_Dimension, MonotonicSet > similarity_map;

    vector < string > attrib_names;

   /**
    * bool is_ready:
    * a boolean value indicating the readiness of the object.
    * The object is usable only if is_ready is true.
    */
    bool is_ready;

    map < vector < unsigned int > , unsigned int, cSimilarity_Compare > x_counts, m_counts;

 /**
 *  void sp_stats (const list<std::pair<string, string> > & trainpairs,
        map < vector < unsigned int > , unsigned int, cSimilarity_Compare > & sp_counts ) const:
 *      read a list of pairs of unique record numbers that are selected as
 *      training sets, and do pairwise comparison in the specified
 *      attribute group. Then the statistics of the appearing similarity
 *      profiles ( part of a complete similarity profile ) are stored
 *      in the map of similarity profiles to their occurrences "sp_counts".
 */
     void sp_stats (const list<std::pair<string, string> > & trainpairs,
       map < vector < unsigned int > , unsigned int, cSimilarity_Compare > & sp_counts ) const;

    void read_train_pairs(list<std::pair<string, string> > & trainpairs, const char * txt_file) const;

    void get_similarity_info();

public:

    class cException_Ratios_Not_Ready : public cAbstract_Exception {
      public:
        cException_Ratios_Not_Ready(const char* errmsg): cAbstract_Exception(errmsg){};
    };

   /**
    *  cRatioComponent ( const map < string, const Record * > uid_tree, const string & groupname ):
    *
    *  @param uid_tree  map of unique record id string to its record pointer.
    *  @param groupname attribute group name.
    */
    explicit cRatioComponent(const map < string, const Record * > & uid_tree, const string & groupname);

   /**
    * TODO: FIXME: Document this method.
    */
    void prepare(const char* x_flie, const char * m_file);

   /**
    * TODO: FIXME: Document this method.
    */
    const map < vector < unsigned int >, double, cSimilarity_Compare > & get_ratios_map() const {
        if ( is_ready )
            return ratio_map;
        else {
            throw cException_Ratios_Not_Ready("Ratio component map is not ready.");
        }
    }

   /**
    * TODO: FIXME: Document this method.
    */
    const map < vector < unsigned int >, unsigned int, cSimilarity_Compare > & get_x_counts() const {
      return x_counts;
    }

   /**
    * TODO: FIXME: Document this method.
    */
    const map < vector < unsigned int >, unsigned int, cSimilarity_Compare > & get_m_counts() const {
      return m_counts;
    }

   /**
    * TODO: FIXME: document this method.
    */
    const vector < unsigned int > & get_component_positions_in_ratios() const {
      return positions_in_ratios;
    };

   /**
    * TODO: FIXME: document this method.
    */
    const vector < unsigned int > & get_component_positions_in_record() const {
      return positions_in_record;
    };

    void smooth();

   /**
    * TODO: FIXME: document this method.
    */
    void stats_output( const char * ) const;

   /**
    * TODO: FIXME: document this method.
    */
    const vector < string > & get_attrib_names() const { return attrib_names;}
};


class cRatios {

private:

   /**
    * final_ratios map takes a similarity vector as a key for the similarity
    * value, with the appropriate similarity comparator.
    */
    map < vector <unsigned int>, double, cSimilarity_Compare > final_ratios;
    vector < string > attrib_names;
    unsigned int ratio_size;
    //vector <double> coeffs;
    //unsigned int final_root_order;
    map < vector < unsigned int > , unsigned int, cSimilarity_Compare > x_counts, m_counts;
    map < cSimilarity_With_Monotonicity_Dimension, MonotonicSet > similarity_map;

    void More_Components( const cRatioComponent & additional_component);
    void Get_Coefficients();
    static const char * primary_delim;
    static const char * secondary_delim;

public:

   /**
    * TODO: FIXME: document this constructor.
    */
    cRatios(const vector < const cRatioComponent *> & component_vector,
          const char * filename,
          const Record & record);

   /**
    * TODO: FIXME: document this constructor.
    */
    cRatios( const char *filename);

   /**
    * The getter for the ratios map, i.e., the lookup table for the
    * computed similarity ratios in Torvik's terminology.
    */
    const map < vector < unsigned int >, double, cSimilarity_Compare > & get_ratios_map() const {
        return final_ratios;
    }


    //const vector <double> & get_coefficients_vector() const { return coeffs;}

   /**
    * The ratios file name is keyed to the current round of disambiguation.
    */
    void read_ratios_file(const char * filename);

   /**
    * TODO: FIXME: document this method.
    */
    void write_ratios_file(const char * filename) const;
    //unsigned int get_final_order () const {return final_root_order;}

   /**
    * TODO: FIXME: document this method.
    */
    void inter_extra_polation(const vector < unsigned int >& max_similarity,
                             const vector < unsigned int > & min_similarity);

   /**
    * Requires global configuration of requisite matrices and data structures
    * for conducting the quadratic programming.
    */
    void smooth();

   /**
    * TODO: FIXME: document this method.
    */
    const vector < string > & get_attrib_names() const { return attrib_names;}
};


vector < unsigned int > get_max_similarity(const vector < string > & attrib_names) ;

#endif /* PATENT_RATIOS_H */
