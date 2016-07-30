#include <cxx_wrap.hpp>
#include <string>
#include <vector>
#include <iostream>

#include "IO/LCReader.h"
#include "IOIMPL/LCFactory.h"
#include "EVENT/LCEvent.h"
#include "EVENT/LCCollection.h"
#include "EVENT/MCParticle.h"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/TrackerHit.h"
#include "EVENT/SimTrackerHit.h"
#include "EVENT/Track.h"
#include "EVENT/ReconstructedParticle.h"
#include "EVENT/Vertex.h"
#include "EVENT/LCRelation.h"
#include "EVENT/LCGenericObject.h"
#include "UTIL/CellIDDecoder.h"
#include "UTIL/BitField64.h"

using namespace std;
using namespace cxx_wrap;

inline IO::LCReader*
createReader() {
    return IOIMPL::LCFactory::getInstance()->createLCReader();
}

inline void
deleteReader(IO::LCReader* reader) {
    delete reader;
}

inline void
openFile(IO::LCReader* reader, const std::string& filename) {
    reader->open(filename);
}

inline EVENT::LCEvent*
readNextEvent(IO::LCReader* reader) {
    return reader->readNextEvent();
}

inline void
closeFile(IO::LCReader* reader) {
    reader->close();
}

// This is just a simple wrapper class around the lccollection pointer
// This will be constructed together with the type, which can be inferred from
// the collection name parameter
template<typename T>
struct TypedCollection
{
    EVENT::LCCollection* m_coll;
    TypedCollection(EVENT::LCCollection* collection) {
        m_coll = collection;
    }
    inline T* getElementAt(size_t i) {
        return static_cast<T*>(m_coll->getElementAt(i));
    }
    inline size_t getNumberOfElements() {
        return m_coll->getNumberOfElements();
    }
    inline EVENT::LCCollection*
    coll() {
        return m_coll;
    }
};

JULIA_CPP_MODULE_BEGIN(registry)
    cxx_wrap::Module& lciowrap = registry.create_module("LCIO");
    lciowrap.add_type<vector<string>>("StringVec")
        .method("size", &EVENT::StringVec::size);
    lciowrap.method("at", [](const vector<string>* vec, size_t i) {
        return vec->at(i);
    });

    lciowrap.add_type<EVENT::LCObject>("LCObject");

    lciowrap.add_type<EVENT::SimCalorimeterHit>("SimCalorimeterHit")
        .method("getEnergy", &EVENT::SimCalorimeterHit::getEnergy);
    // returns a bool to indicate whether this was a nullptr
    lciowrap.method("getP3", [](const EVENT::SimCalorimeterHit* hit, ArrayRef<double> x)->bool {
            const float* p = hit->getPosition();
            if (not p) {return false;}
            x[0] = p[0];
            x[1] = p[1];
            x[2] = p[2];
            return true;
        });

    lciowrap.add_type<EVENT::SimTrackerHit>("SimTrackerHit");

    lciowrap.add_type<EVENT::CalorimeterHit>("CalorimeterHit");

    lciowrap.add_type<EVENT::TrackerHit>("TrackerHit");

    lciowrap.add_type<EVENT::MCParticle>("MCParticle")
        .method("getEnergy", &EVENT::MCParticle::getEnergy)
        .method("getPDG", &EVENT::MCParticle::getPDG)
        .method("getGeneratorStatus", &EVENT::MCParticle::getGeneratorStatus)
        .method("getSimulatorStatus", &EVENT::MCParticle::getSimulatorStatus)
        .method("isCreatedInSimulation", &EVENT::MCParticle::isCreatedInSimulation)
        .method("isBackscatter", &EVENT::MCParticle::isBackscatter)
        .method("vertexIsNotEndpointOfParent", &EVENT::MCParticle::vertexIsNotEndpointOfParent)
        .method("isDecayedInTracker", &EVENT::MCParticle::isDecayedInTracker)
        .method("isDecayedInCalorimeter", &EVENT::MCParticle::isDecayedInCalorimeter)
        .method("hasLeftDetector", &EVENT::MCParticle::hasLeftDetector)
        .method("isStopped", &EVENT::MCParticle::isStopped)
        .method("isOverlay", &EVENT::MCParticle::isOverlay)
        .method("getTime", &EVENT::MCParticle::getTime)
        .method("getMass", &EVENT::MCParticle::getMass)
        .method("getCharge", &EVENT::MCParticle::getCharge);

    lciowrap.add_type<EVENT::MCParticleVec>("MCParticleVec")
        .method("size", &EVENT::MCParticleVec::size);
    lciowrap.method("at", [](const EVENT::MCParticleVec& vec, size_t i) {
            return vec.at(i);
    });

    lciowrap.method("getParents", [](const EVENT::MCParticle* p)->const EVENT::MCParticleVec& {
        return p->getParents();
    });
    lciowrap.method("getDaughters", [](const EVENT::MCParticle* p)->const EVENT::MCParticleVec& {
        return p->getDaughters();
    });
    // returns a bool to indicate whether this was a nullptr
    lciowrap.method("getVertex3", [](const EVENT::MCParticle* p, ArrayRef<double> v)->bool {
        const double* vtx = p->getVertex();
        if (not vtx) {return false;}
        v[0] = vtx[0];
        v[1] = vtx[1];
        v[2] = vtx[2];
        return true;
    });
    // returns a bool to indicate whether this was a nullptr
    lciowrap.method("getEndpoint3", [](const EVENT::MCParticle* p, ArrayRef<double> pos)->bool {
        const double* endp = p->getEndpoint();
        if (not endp) {return false;}
        pos[0] = endp[0];
        pos[1] = endp[1];
        pos[2] = endp[2];
        return true;
    });
    // returns a bool to indicate whether this was a nullptr
    lciowrap.method("getMomentum3", [](const EVENT::MCParticle* p, ArrayRef<double> p3)->bool {
        const double* mom = p->getMomentum();
        if (not mom) {return false;}
        p3[0] = mom[0];
        p3[1] = mom[1];
        p3[2] = mom[2];
        return true;
    });
    // returns a bool to indicate whether this was a nullptr
    lciowrap.method("getMomentumAtEndpoint3", [](const EVENT::MCParticle* p, ArrayRef<double> p3)->bool {
        const double* mom = p->getMomentumAtEndpoint();
        if (not mom) {return false;}
        p3[0] = mom[0];
        p3[1] = mom[1];
        p3[2] = mom[2];
        return true;
    });

    lciowrap.add_type<EVENT::LCRelation>("LCRelation");

    lciowrap.add_type<EVENT::Vertex>("Vertex");

    lciowrap.add_type<EVENT::Track>("Track");

    lciowrap.add_type<EVENT::LCGenericObject>("LCGenericObject");

    lciowrap.add_type<EVENT::ReconstructedParticle>("ReconstructedParticle");

    // most of the functionality is forwarded to the TypedCollection
    lciowrap.add_type<EVENT::LCCollection>("LCCollection")
        .method("getNumberOfElements", &EVENT::LCCollection::getNumberOfElements)
        .method("getElementAt", &EVENT::LCCollection::getElementAt)
        .method("getTypeName", &EVENT::LCCollection::getTypeName);

    lciowrap.add_type<EVENT::LCEvent>("LCEvent")
        .method("getEventCollection", &EVENT::LCEvent::getCollection)
        .method("getCollectionNames", &EVENT::LCEvent::getCollectionNames)
        .method("getDetectorName", &EVENT::LCEvent::getDetectorName)
        .method("getEventNumber", &EVENT::LCEvent::getEventNumber)
        .method("getRunNumber", &EVENT::LCEvent::getRunNumber);
    //
    lciowrap.add_type<IO::LCReader>("LCReader")
      .method("getNumberOfEvents", &IO::LCReader::getNumberOfEvents);
        // .method("open", &IO::LCReader::open)
        // .method("readNextEvent", &IO::LCReader::readNextEvent)
        // .method("close", &IO::LCReader::close);
    lciowrap.method("createLCReader", &createReader);
    lciowrap.method("deletLCReader", &deleteReader);
    lciowrap.method("openFile", &openFile);
    lciowrap.method("readNextEvent", &readNextEvent);
    lciowrap.method("closeFile", &closeFile);


    lciowrap.add_type<Parametric<TypeVar<1>>>("TypedCollection")
        .apply<TypedCollection<EVENT::SimCalorimeterHit>
             , TypedCollection<EVENT::CalorimeterHit>
             , TypedCollection<EVENT::MCParticle>
             , TypedCollection<EVENT::ReconstructedParticle>
             , TypedCollection<EVENT::TrackerHit>
             , TypedCollection<EVENT::SimTrackerHit>
             , TypedCollection<EVENT::LCRelation>
             , TypedCollection<EVENT::LCGenericObject>
             , TypedCollection<EVENT::Track>
             , TypedCollection<EVENT::Vertex>>([](auto wrapped)
        {
        typedef typename decltype(wrapped)::type WrappedColl;
        wrapped.template constructor<EVENT::LCCollection*>();
        wrapped.method("getElementAt", &WrappedColl::getElementAt);
        wrapped.method("getNumberOfElements", &WrappedColl::getNumberOfElements);
        wrapped.method("coll", &WrappedColl::coll);
    });

    lciowrap.add_type<UTIL::BitField64>("BitField64")
        .constructor<const string&>()
        .method("getValue", &UTIL::BitField64::getValue);

    lciowrap.add_type<Parametric<TypeVar<1>>>("CellIDDecoder")
        .apply<UTIL::CellIDDecoder<EVENT::SimCalorimeterHit>
             , UTIL::CellIDDecoder<EVENT::CalorimeterHit>
             , UTIL::CellIDDecoder<EVENT::TrackerHit>
             , UTIL::CellIDDecoder<EVENT::SimTrackerHit>>([](auto wrapped)
    {
        typedef typename decltype(wrapped)::type WrappedT;
        wrapped.template constructor<const EVENT::LCCollection*>();
        // wrapped.template constructor<const string&>();
        wrapped.method("get", &WrappedT::operator());
    });

JULIA_CPP_MODULE_END
