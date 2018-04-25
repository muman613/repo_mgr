/*
    This file is part of crepo.

    crepo is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    crepo is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with crepo.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "StdAfx.h"
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <unistd.h>
#include "crepo.h"
#include "repoObject.h"
#include "functions.h"
#include "utils.h"

repoRemote::repoRemote(STRING name, STRING fetch, STRING review)
:   m_sName(name), m_sFetch(fetch), m_sReview(review)
{
    //ctor
}

repoRemote::repoRemote()
{
    //ctor
}

repoRemote::~repoRemote()
{
    //dtor
}

STRING repoRemote::Name() const {
    return m_sName;
}

STRING repoRemote::Fetch() const {
    return m_sFetch;
}

STRING repoRemote::Review() const {
    return m_sReview;
}

/**
 *
 */

repoProject::repoProject(STRING name, STRING remote, STRING path)
:   m_sName(name), m_sRemote(remote), m_sPath(path)
{
    //ctor
}

repoProject::repoProject()
{

}

repoProject::~repoProject() {
    //dtor
}
STRING      repoProject::Name() const {
    return m_sName;
}
STRING      repoProject::Remote() const {
    return m_sRemote;
}
STRING      repoProject::Path() const {
    return m_sPath;
}

repoObject::repoObject(STRING sCheckoutPath)
:   m_sCheckoutPath(sCheckoutPath)
{
    //ctor
    printf("checkout path %s\n", sCheckoutPath.c_str());
}

repoObject::repoObject()
{
    //ctor
}

repoObject::~repoObject()
{
    //dtor
}

void repoObject::AddRemote(STRING name, STRING fetch, STRING review) {
#ifdef  _DEBUG
    fprintf(stderr,"adding remote %s\n", name.c_str());
#endif // _DEBUG

    m_remotes.push_back(repoRemote(name, fetch, review));
}

void repoObject::AddRemote(xmlNodePtr pNode) {
    STRING      sName, sFetch, sReview;
    xmlChar*    szTmp = nullptr;

    if ((szTmp = xmlGetProp(pNode, BAD_CAST "name")) != nullptr) {
        sName = (const char *)szTmp;
    }
    if ((szTmp = xmlGetProp(pNode, BAD_CAST "fetch")) != nullptr) {
        sFetch = (const char*)szTmp;
    }
    if ((szTmp = xmlGetProp(pNode, BAD_CAST "review")) != nullptr) {
        sReview = (const char*)szTmp;
    }

    if (!sName.empty() && !sFetch.empty() && !sReview.empty()) {
        AddRemote(sName, sFetch, sReview);
    }

    return;
}

void repoObject::AddProject(STRING sName, STRING sRemote, STRING sPath) {
#ifdef  _DEBUG
    fprintf(stderr, "Adding project %s\n", sName.c_str());
#endif // _DEBUG

    m_projects.push_back(repoProject(sName, sRemote, sPath));
}

void repoObject::AddProject(xmlNodePtr pNode) {
    STRING      sName, sRemote, sPath;
    xmlChar*    szTmp = nullptr;

    if ((szTmp = xmlGetProp(pNode, BAD_CAST "name")) != nullptr) {
        sName = (const char *)szTmp;
    }
    if ((szTmp = xmlGetProp(pNode, BAD_CAST "remote")) != nullptr) {
        sRemote = (const char*)szTmp;
    }
    if ((szTmp = xmlGetProp(pNode, BAD_CAST "path")) != nullptr) {
        sPath = (const char*)szTmp;
    }

    if (!sName.empty() && !sRemote.empty() && !sPath.empty()) {
        AddProject(sName, sRemote, sPath);
    }

    return;
}


bool repoObject::getProjectByName(STRING sName, PROJECT& project) {
    bool bRes = false;
    return bRes;
}

bool repoObject::getRemoteByName(STRING sName, REMOTE&  remote) {
    for (auto it = m_remotes.begin() ; it != m_remotes.end() ; it++) {
        if ((*it).Name() == sName) {
            remote = *it;
            return true;
        }
    }
    return false;
}

bool repoObject::LoadRepoFromXML(STRING sRepoXML) {
    bool        bRes    = false;
    xmlDoc*     doc     = nullptr;

    m_remotes.clear();
    m_projects.clear();

    if ((doc = xmlReadFile(sRepoXML.c_str(), NULL, 0)) != nullptr) {
        xmlNode*    pRoot = nullptr;

        pRoot = xmlDocGetRootElement( doc );

        if (pRoot != nullptr) {
#ifdef  _DEBUG
            fprintf(stderr, "root note = %s\n", pRoot->name);
#endif // _DEBUG
            if (xmlStrcasecmp(pRoot->name, BAD_CAST "manifest") == 0) {
                xmlNodePtr  pNode = pRoot->children;

                while (pNode != nullptr) {
                    if (pNode->type == XML_ELEMENT_NODE) {
#ifdef  _DEBUG
                        fprintf(stderr, "name = %s\n", pNode->name);
#endif // _DEBUG
                        if (xmlStrcasecmp(pNode->name, BAD_CAST "remote") == 0) {
                            AddRemote(pNode);
                        } else if (xmlStrcasecmp(pNode->name, BAD_CAST "default") == 0) {

                        } else if (xmlStrcasecmp(pNode->name, BAD_CAST "project") == 0) {
                            AddProject(pNode);
                        }
                    }
                    pNode = pNode->next;
                }
            }
            bRes = true;
#ifdef  _DEBUG
            fprintf(stderr, "# of remotes  : %ld\n", m_remotes.size());
            fprintf(stderr, "# of projects : %ld\n", m_projects.size());
#endif
        }

        xmlFreeDoc( doc );
    } else {
        fprintf(stderr, "ERROR: Unable to open XML file!\n");
    }

    xmlCleanupParser();

    return bRes;
}

/**
 *
 **/

bool repoObject::CheckoutRepo() {
    //bool bRes = false;
    repoRemote remote;
    STRING sOldPath;
//    char* szTmp = nullptr;

    /* Change directory into checkout path */
    if (!m_sCheckoutPath.empty()) {

        get_current_directory(sOldPath);

        if (chdir(m_sCheckoutPath.c_str()) != 0) {
            return false;
        }
    }

    for (auto it = m_projects.begin() ; it != m_projects.end() ; it++) {
        repoProject project = (*it);

#ifdef  _DEBUG
        fprintf(stderr, "PROJECT Name %s Remote %s Path %s\n",
                project.Name().c_str(),
                project.Remote().c_str(),
                project.Path().c_str());
#endif // _DEBUG

        if (getRemoteByName(project.Remote(), remote)) {
#ifdef  _DEBUG
            fprintf(stderr, "found remote %s\n", remote.Name().c_str());
#endif // _DEBUG

            git_checkout(remote.Fetch() + project.Name(), "", project.Path());
        } else {
            fprintf(stderr, "ERROR: Unable to find remote %s!\n",
                    project.Remote().c_str());
        }
    }

    /* Restore the current directory path */
    if (!m_sCheckoutPath.empty()) {
        if (chdir(sOldPath.c_str()) != 0) {
            fprintf(stderr, "WARNING: Unable to restore directory!\n");
        }
    }

    return true;
}

/**
 *      Perform git diff on repositories.
 **/

bool repoObject::DiffRepo() {
    STRING sOldPath, sCmd;

    get_current_directory(sOldPath);

    for (auto it = m_projects.begin() ; it != m_projects.end() ; it++) {
        repoProject project = (*it);

        if (it != m_projects.begin())
            printf("\n");

        if (isatty(fileno(stdout))) {
            printf("%sChanging directory to %s'%s'%s:\n",
                    ansiBoldCyan,
                    ansiBoldGreen,
                    project.Path().c_str(),
                    ansiReset);
        } else {
            printf("Changing directory to '%s':\n",
                    project.Path().c_str());
        }

        if (chdir(project.Path().c_str()) !=0) {
            fprintf(stderr, "WARNING: repository %s not found!\n", project.Path().c_str());
            continue;
        }

        if (isatty(fileno(stdout))) {
            sCmd = "git -c color.diff=always diff";
        } else {
            sCmd = "git diff";
        }

        execute_shell_command(sCmd);

        if (chdir(sOldPath.c_str()) != 0) {
            fprintf(stderr, "WARNING: Unable to restore directory!\n");
        }
    }

    return false;
}

/**
 *      Perform git status on repositories.
 **/

bool repoObject::StatRepo() {
    STRING sOldPath, sCmd;

    get_current_directory(sOldPath);

    for (auto it = m_projects.begin() ; it != m_projects.end() ; it++) {
        repoProject project = (*it);

        if (it != m_projects.begin())
            printf("\n");

        if (isatty(fileno(stdout))) {
            printf("%sChanging directory to %s'%s'%s:\n",
                    ansiBoldCyan,
                    ansiBoldGreen,
                    project.Path().c_str(),
                    ansiReset);
        } else {
            printf("Changing directory to '%s':\n",
                    project.Path().c_str());
        }

        if (chdir(project.Path().c_str()) !=0) {
            fprintf(stderr, "WARNING: repository %s not found!\n", project.Path().c_str());
            continue;
        }

        sCmd = "git -c color.status=always status -uno";

        execute_shell_command(sCmd);

        if (chdir(sOldPath.c_str()) != 0) {
            fprintf(stderr, "WARNING: Unable to restore directory!\n");
        }
    }

    return false;
}

/**
 *      Perform git status on repositories.
 **/

bool repoObject::BranchRepo() {
    STRING sOldPath, sCmd;

    get_current_directory(sOldPath);

    for (auto it = m_projects.begin() ; it != m_projects.end() ; it++) {
        repoProject project = (*it);

        if (it != m_projects.begin())
            printf("\n");

        if (isatty(fileno(stdout))) {
            printf("%sChanging directory to %s'%s'%s:\n",
                    ansiBoldCyan,
                    ansiBoldGreen,
                    project.Path().c_str(),
                    ansiReset);
        } else {
            printf("Changing directory to '%s':\n",
                    project.Path().c_str());
        }

        if (chdir(project.Path().c_str()) !=0) {
            fprintf(stderr, "WARNING: repository %s not found!\n", project.Path().c_str());
            continue;
        }

        sCmd = "git br";

        execute_shell_command(sCmd);

        if (chdir(sOldPath.c_str()) != 0) {
            fprintf(stderr, "WARNING: Unable to restore directory!\n");
        }
    }

    return false;
}

bool repoObject::TagRepo(STRING sTag) {
    STRING sOldPath, sCmd;

    if (sTag.empty()) {
        printf("ERROR: Must specify tag!\n");
        return false;
    }

    get_current_directory(sOldPath);

    for (auto it = m_projects.begin() ; it != m_projects.end() ; it++) {
        repoProject project = (*it);

        if (it != m_projects.begin())
            printf("\n");

        if (isatty(fileno(stdout))) {
            printf("%sChanging directory to %s'%s'%s:\n",
                    ansiBoldCyan,
                    ansiBoldGreen,
                    project.Path().c_str(),
                    ansiReset);
        } else {
            printf("Changing directory to '%s':\n",
                    project.Path().c_str());
        }

        if (chdir(project.Path().c_str()) !=0) {
            fprintf(stderr, "WARNING: repository %s not found!\n", project.Path().c_str());
            continue;
        }

        printf("Setting tag to %s.\n", sTag.c_str());

        sCmd = "git tag " + sTag;

        execute_shell_command(sCmd);

        if (chdir(sOldPath.c_str()) != 0) {
            fprintf(stderr, "WARNING: Unable to restore directory!\n");
        }
    }

    return false;
}

/**
 *      Perform git pull origin on repositories.
 **/

bool repoObject::SyncRepo() {
    STRING sOldPath, sCmd;

    get_current_directory(sOldPath);

    for (auto it = m_projects.begin() ; it != m_projects.end() ; it++) {
        repoProject project = (*it);

        if (it != m_projects.begin())
            printf("\n");

        if (isatty(fileno(stdout))) {
            printf("%sChanging directory to %s'%s'%s:\n",
                    ansiBoldCyan,
                    ansiBoldGreen,
                    project.Path().c_str(),
                    ansiReset);
        } else {
            printf("Changing directory to '%s':\n",
                    project.Path().c_str());
        }

        if (chdir(project.Path().c_str()) !=0) {
            fprintf(stderr, "WARNING: repository %s not found!\n", project.Path().c_str());
            continue;
        }

        sCmd = "git pull origin master";

        execute_shell_command(sCmd);

        if (chdir(sOldPath.c_str()) != 0) {
            fprintf(stderr, "WARNING: Unable to restore directory!\n");
        }
    }

    return true;
}
