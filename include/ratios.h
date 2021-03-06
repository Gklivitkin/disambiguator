
#ifndef PATENT_RATIOS_H
#define PATENT_RATIOS_H

#include <iostream>
#include <map>
#include <set>
#include <fstream>

#include "typedefs.h"
#include "disambiguation.h"
#include "attribute.h"

using std::string;
using std::set;
using std::map;

typedef std::pair<string, string> TrainingPair;
typedef std::list<TrainingPair> TrainingPairs;

typedef map<string, const Record *> RecordIndex;

typedef map<SimilarityProfile, double, SimilarityCompare> SPRatiosIndex;
typedef map<SimilarityProfile, sp_count_t, SimilarityCompare> SPCountsIndex;


/**
 * For two given similarity profiles, it only compares the
 * scores at a certain position.
 *
 * This class should ONLY serve as a comparison functor
 * for associated containers, i.e., map or set.
 */
struct cMonotonic_Similarity_Compare {

private:

   /**
    * uint32_t compare_entry: the position of interest.
    */
    uint32_t compare_entry;

public:

   /**
    * bool operator() (const SimilarityProfile * p1, const SimilarityProfile * p2) const:
    * to compare the two similarity profiles at the position of compare_entry.
    */
    bool operator() (const SimilarityProfile * p1, const SimilarityProfile * p2) const {
        return p1->at(compare_entry) < p2->at(compare_entry);
    }


   /**
    * cMonotonic_Similarity_Compare(const uint32_t entry): constructor
    */
    cMonotonic_Similarity_Compare(const uint32_t entry)
      : compare_entry(entry) {};

   /**
    * void reset_entry(const uint32_t entry):
    * reset the variable compare_entry to the input entry.
    */
    void reset_entry(const uint32_t entry) {
      compare_entry = entry;
    }
};


/**
 * MonotonicSet:
 * members in this set is sorted by a given similarity entry in an ascending way.
 */
typedef set<const SimilarityProfile *, cMonotonic_Similarity_Compare> MonotonicSet;


/**
 * cSimilarity_With_Monotonicity_Dimension:
 *
 * This class wraps a similarity profile for a map or
 * set structure. The primary purpose of the class
 * is to allow comparison of similarity profiles
 * skipping a certain entry in the profile.
 *
 * Use of the above classes is primarily in the DisambigRatioSmoothing.cpp.
 * It is not expected to have a very solid understanding of the above
 * classes, unless smoothing, interpolation and extrapolation of
 * similarity profiles are supposed to change.
 */
struct cSimilarity_With_Monotonicity_Dimension {

private:

   /**
    * Private:
    * const SimilarityProfile * psim: pointer to a similarity profile.
    */
    const SimilarityProfile * psim;

   /**
    * uint32_t monotonic_dimension: an entry in which
    * comparison of similarity profiles will skip.
    */
    uint32_t monotonic_dimension;

   /**
    * bool compare_without_primary( const SimilarityProfile * p1, const SimilarityProfile * p2 ) const:
    *     compare the similarity profiles in all dimensions except the "monotonic_dimension" dimension.
    */
    bool compare_without_primary(const SimilarityProfile * p1,
        const SimilarityProfile * p2 ) const;

public:

   /**
    * bool operator < ( const cSimilarity_With_Monotonicity_Dimension & rhs) const:
    * comparison function that is used only in map/set.
    */
    bool operator < (const cSimilarity_With_Monotonicity_Dimension & rhs) const;

   /**
    * const uint32_t get_monotonic_dimension() const: return the monotunic_dimension
    */
    //const uint32_t get_monotonic_dimension() const {
    uint32_t get_monotonic_dimension() const {
      return monotonic_dimension;
    }

   /**
    * cSimilarity_With_Monotonicity_Dimension( const SimilarityProfile * p, const uint32_t dm ):
    * constructor.
    */
    explicit cSimilarity_With_Monotonicity_Dimension(
        const SimilarityProfile * p, const uint32_t dm)
        : psim (p), monotonic_dimension(dm) {}
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
class cRatioComponent {

    friend class RatioComponentTest;

    class cException_Partial_SP_Missing : public cAbstract_Exception {
    public:
        cException_Partial_SP_Missing(const char* errmsg)
          : cAbstract_Exception(errmsg){};
    };

private:

   /**  static const uint32_t laplace_base:
    *  a value used for laplacian operations of
    *  obtained similarity profiles to get a ratio.
    */
    static const uint32_t laplace_base;

   /**
    *  map < SimilarityProfile, double, SimilarityCompare > ratio_map:
    *  a ratio map for the current component.
    *  Key = similarity profile,
    *  Value = ratio,
    *  Comparator = SimilarityCompare
    */
    //map < SimilarityProfile, double, SimilarityCompare > ratio_map;
    // SPRatiosIndex
    //map< SimilarityProfile, double, SimilarityCompare > ratio_map;
    SPRatiosIndex ratio_map;

   /**
    * vector<uint32_t> positions_in_ratios:
    * positions of the current components in the complete similarity profile.
    */
    vector<uint32_t> positions_in_ratios;

   /**
    * vector<uint32_t> positions_in_record:
    * position of the current components in the Record::column_names.
    */
    vector<uint32_t> positions_in_record;

   /**
    * const string attrib_group:
    * the attribute GROUP identifier for which the
    * cRatioComponent object represents.
    */
    const string attrib_group;

   /**
    * const map < string, const Record *> * puid_tree:
    * the pointer to a map of unique record id string to
    * its correspoinding record pointer.
    */
    //const map<string, const Record *> * puid_tree;
    const RecordIndex * puid_tree;


   /**
    * map<cSimilarity_With_Monotonicity_Dimension, MonotonicSet > similarity_map:
    * a map of similarity profiles and their monotonic set for a certain dimension.
    */
    map<cSimilarity_With_Monotonicity_Dimension, MonotonicSet> similarity_map;

   /**
    * vector<string> attrib_names:
    * attribute names that belong to the atribute group.
    */
    vector<string> attrib_names;

   /**
    * bool is_ready:
    * a boolean value indicating the readiness of the object.
    * The object is usable only if is_ready is true.
    */
    bool is_ready;

   /**
    * map<vector<uint32_t>, uint32_t, SimilarityCompare> x_counts, m_counts:
    * maps of similarity profiles to their occurrences in non-match and match training sets.
    */
    //map <vector<uint32_t>, uint32_t, SimilarityCompare > x_counts, m_counts;
    SPCountsIndex x_counts, m_counts;

   /**
    *  void sp_stats (const list<std::pair<string, string> > & trainpairs,
    *   map < vector < uint32_t > , uint32_t, SimilarityCompare > & sp_counts ) const:
    *   read a list of pairs of unique record numbers that are selected as
    *   training sets, and do pairwise comparison in the specified
    *   attribute group. Then the statistics of the appearing similarity
    *   profiles ( part of a complete similarity profile ) are stored
    *   in the map of similarity profiles to their occurrences "sp_counts".
    */
    void sp_stats (const TrainingPairs & trainpairs, SPCountsIndex & sp_counts) const;

   /**
    * Use the counts to create the ratios, store into the similarity profile
    * database/lookup table.
    */
    void create_ratios();

   /**
    *  void read_train_pairs ( list < std::pair < string, string > & trainpairs, const char * txt_file ) const:
    *      read the list of pairs of unique record numbers ( training sets ) from the
    *      specified "txt_file" into the list "trainpairs".
    */
    //void read_train_pairs(list<std::pair<string, string> > & trainpairs, const char * txt_file) const;
    void read_train_pairs(TrainingPairs & trainpairs, const char * txt_file) const;

   /**
    *  void get_similarity_info():
    *   to get the information of similarity profiles of the attribute group.
    */
    void get_similarity_info();

public:

    class cException_Ratios_Not_Ready : public cAbstract_Exception {
      public:
        cException_Ratios_Not_Ready(const char* errmsg)
          : cAbstract_Exception(errmsg) {};
    };

   /**
    *  cRatioComponent (const map < string, const Record * > uid_tree, const string & groupname):
    *
    *  @param uid_tree  map of unique record id string to its record pointer.
    *  @param groupname attribute group name.
    */
    explicit cRatioComponent(const RecordIndex & uid_tree, const string & groupname);

   /**
    * `prepare` is a wrapper function which sets up all the ratios and
    * similarities before feeding everything into the QP for interpolation
    * and extrapolation of missing similarity and ratio values.
    */
    void prepare(const char* x_flie, const char * m_file);

   /**
    * Laplace correction for handling missing similarity profiles.
    * Look for the `laplace_base` and `laplace_max_count` variables
    * for setting the parameter values appropriately.
    */
    void laplace_correction();

    const SPRatiosIndex & get_ratios_map() const {

        if (is_ready) {
            return ratio_map;
        } else {
            throw cException_Ratios_Not_Ready("Ratio component map is not ready.");
        }
    }

    const SPCountsIndex & get_x_counts() const {
      return x_counts;
    }

    const SPCountsIndex & get_m_counts() const {
      return m_counts;
    }

    const vector<uint32_t> & get_component_positions_in_ratios() const {
      return positions_in_ratios;
    };

    const vector<uint32_t> & get_component_positions_in_record() const {
      return positions_in_record;
    };

    void smooth();

   /**
    * Write count statistics to a file.
    */
    void stats_output(const char *) const;

    const vector<string> & get_attrib_names() const {
      return attrib_names;
    }
};


class cRatios {

private:

   /**
    * final_ratios map takes a similarity vector as a key for the similarity
    * value, with the appropriate similarity comparator.
    */
    SPRatiosIndex final_ratios;

    vector<string> attrib_names;

    uint32_t ratio_size;

    SPCountsIndex x_counts, m_counts;

    map<cSimilarity_With_Monotonicity_Dimension, MonotonicSet > similarity_map;

    void More_Components(const cRatioComponent & additional_component);

    void Get_Coefficients();

    static const char * primary_delim;

    static const char * secondary_delim;

public:

   /**
    * TODO: FIXME: document this constructor.
    */
    cRatios(const vector<const cRatioComponent *> & component_vector,
            const char * filename,
            const Record & record);

   /**
    * TODO: FIXME: document this constructor.
    */
    cRatios(const char *filename);

   /**
    * The getter for the ratios map, i.e., the lookup table for the
    * computed similarity ratios in Torvik's terminology.
    */
    const SPRatiosIndex & get_ratios_map() const {
        return final_ratios;
    }


   /**
    * The ratios file name is keyed to the current round of disambiguation.
    */
    void read_ratios_file(const char * filename);

   /**
    * TODO: FIXME: document this method.
    */
    void write_ratios_file(const char * filename) const;

   /**
    * Requires global configuration of requisite matrices and data structures
    * for conducting the quadratic programming.
    */
    void smooth();

   /**
    * TODO: FIXME: document this method.
    */
    const vector<string> & get_attrib_names() const {
      return attrib_names;
    }
};


vector<uint32_t> get_max_similarity                   (const vector<string> & attrib_names);

const Record *   retrieve_record_pointer_by_unique_id (const string & uid,
                                                       const RecordIndex & uid_tree);

void             create_btree_uid2record_pointer      (RecordIndex & uid_tree,
                                                       const list<Record> & reclist,
                                                       const string & uid_name );

bool             dump_match                           (const char * sqlite3_target,
                                                       const char * tablename,
                                                       const char * txt_source,
                                                       const string & unique_record_name,
                                                       const string & unique_inventor_name);

uint32_t         compute_total_nodes                  (const SimilarityProfile & min_sp,
                                                       const SimilarityProfile & max_sp);

void             print_similarity                     (const SimilarityProfile & s);

void             print_similarity_profile_size        (const SimilarityProfile & s);


/**
 * TODO: Probably ought to be private or protected.
 */
void             print_map                            (SPRatiosIndex & m);


uint32_t         sp2index                             (const SimilarityProfile & sp,
                                                       const SimilarityProfile & min_sp,
                                                       const SimilarityProfile & max_sp);

SimilarityProfile index2sp                            (uint32_t index,
                                                       const SimilarityProfile & min_sp,
                                                       const SimilarityProfile & max_sp);

void             smoothing_inter_extrapolation_cplex  (SPRatiosIndex & ratio_map,
                                                       const SimilarityProfile & min_sp,
                                                       const SimilarityProfile & max_sp,
                                                       const SPCountsIndex & x_counts,
                                                       const SPCountsIndex & m_counts,
                                                       const vector<string> & attribute_names, // TODO: Delete this argument, not used
                                                       const bool name_range_check,            // TODO: Not used
                                                       const bool backup_quadprog);          // TODO: Not used



#endif /* PATENT_RATIOS_H */
