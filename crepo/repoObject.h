#ifndef REPOOBJECT_H
#define REPOOBJECT_H

#include <string>
#include <vector>
#include "crepo.h"

struct _xmlNode;

/**
 *
 */

class repoRemote {
public:
    repoRemote(STRING name, STRING fetch, STRING review);
    repoRemote();
    virtual ~repoRemote();

    STRING      Name() const;
    STRING      Fetch() const;
    STRING      Review() const;

protected:
    STRING      m_sName;
    STRING      m_sFetch;
    STRING      m_sReview;
};

typedef repoRemote                          REMOTE;
typedef std::vector<repoRemote>             REMOTE_VECTOR;
typedef std::vector<repoRemote>::iterator   REMOTE_VECTOR_ITER;

/**
 *
 */

class repoProject {
public:
    repoProject(STRING name, STRING remote, STRING path);
    repoProject();
    virtual ~repoProject();

    STRING      Name() const;
    STRING      Remote() const;
    STRING      Path() const;


protected:
    STRING      m_sName;
    STRING      m_sRemote;
    STRING      m_sPath;
};

typedef repoProject                         PROJECT;
typedef std::vector<repoProject>            PROJECT_VECTOR;
typedef std::vector<repoProject>::iterator  PROJECT_VECTOR_ITER;

/**
 *
 */

class repoObject
{
    public:
        repoObject(STRING sCheckoutPath);
        repoObject();
        virtual ~repoObject();

        bool                LoadRepoFromXML(STRING sRepoXML);
        bool                CheckoutRepo();
        bool                DiffRepo();
        bool                StatRepo();
        bool                BranchRepo();
        bool                TagRepo(STRING sTag);
        bool                SyncRepo();

    protected:

        void                AddRemote(STRING name, STRING fetch, STRING review);
        void                AddRemote(_xmlNode* pNode);
        void                AddProject(STRING sName, STRING sRemote, STRING sPath);
        void                AddProject(_xmlNode* pNode);

        bool                getProjectByName(STRING sName, PROJECT& project);
        bool                getRemoteByName(STRING sName, REMOTE& remote);

        STRING              m_sCheckoutPath;
        REMOTE_VECTOR       m_remotes;
        PROJECT_VECTOR      m_projects;
    private:
};

#endif // REPOOBJECT_H
