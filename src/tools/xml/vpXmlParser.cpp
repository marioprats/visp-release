/****************************************************************************
 *
 * $Id: vpXmlParser.cpp 3834 2012-07-03 10:36:30Z fspindle $
 *
 * This file is part of the ViSP software.
 * Copyright (C) 2005 - 2012 by INRIA. All rights reserved.
 * 
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * ("GPL") version 2 as published by the Free Software Foundation.
 * See the file LICENSE.txt at the root directory of this source
 * distribution for additional information about the GNU GPL.
 *
 * For using ViSP with software that can not be combined with the GNU
 * GPL, please contact INRIA about acquiring a ViSP Professional 
 * Edition License.
 *
 * See http://www.irisa.fr/lagadic/visp/visp.html for more information.
 * 
 * This software was developed at:
 * INRIA Rennes - Bretagne Atlantique
 * Campus Universitaire de Beaulieu
 * 35042 Rennes Cedex
 * France
 * http://www.irisa.fr/lagadic
 *
 * If you have questions regarding the use of this file, please contact
 * INRIA at visp@inria.fr
 * 
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 * Description:
 * Tool to automatise the creation of xml parser based on the libXML2
 *
 * Authors:
 * Romain Tallonneau
 *
 *****************************************************************************/
 
#include <visp/vpXmlParser.h>
#include <visp/vpConfig.h>

#ifdef VISP_HAVE_XML2

#include <visp/vpException.h>
#include <visp/vpDebug.h>
#include <libxml/parser.h>

#include <string>
#include <sstream>
#include <iomanip>
#include <typeinfo>

/*!
  Basic constructor.
  
  Initialise the main tag with default value.
*/
vpXmlParser::vpXmlParser()
{
  main_tag = "config";
}

/*!
  Basic destructor that does nothing.

  \warning As stated in http://xmlsoft.org/html/libxml-parser.html#xmlCleanupParser
  to clean up memory allocated by the xml2 library itself, the user should call
  xmlCleanupParser() only when the process has finished using the xml2 library.
  In case of doubt abstain from calling this function or do it just before calling exit()
  to avoid leak reports from valgrind ! That's why
  in ViSP the destructor doesn't call xmlCleanupParser(). Rather we provide the static
  function vpXmlParser::cleanup() that calls xmlCleanupParser() that could be called
  just before exit().
  
  \sa cleanup()
*/
vpXmlParser::~vpXmlParser()
{
  //xmlCleanupParser();
}

/*!
  Cpoy constructor. 
  
  \param _twin : The parser to copy.
*/
vpXmlParser::vpXmlParser(const vpXmlParser& _twin)
{
  main_tag = _twin.main_tag;
  nodeMap = _twin.nodeMap;
}

/* utilities functions to read/write data from an xml document */

/*!
  Read an array of character. 
  
  \warning The array of characters is allocated and must be explicitly freed to 
  avoid memory leak. 
  
  \param doc : The main xml document
  \param node : a pointer to the node to read value
  
  \return pointer to an allocated array of character.
*/
char* 
vpXmlParser::xmlReadCharChild (xmlDocPtr doc, xmlNodePtr node)
{
  if(node ->xmlChildrenNode == NULL){
    std::string errorMsg = "Empty node " + std::string((char*)node->name) + ", cannot read char*";
    std::cerr << errorMsg << std::endl;
    throw vpException(vpException::fatalError, errorMsg);
  }
  return (char *) xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
}

/*!
  Read an array of character.


  \param doc : The main xml document
  \param node : a pointer to the node to read value

  \return std::string representing the value.
*/

std::string
vpXmlParser::xmlReadStringChild (xmlDocPtr doc, xmlNodePtr node)
{
  if(node ->xmlChildrenNode == NULL){
    std::string errorMsg = "Empty node " + std::string((char*)node->name) + ", cannot read std::string";
    std::cerr << errorMsg << std::endl;
    throw vpException(vpException::fatalError, errorMsg);
  }
  char* dataCh = (char*)xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
  std::string dataStr = dataCh;
  xmlFree(dataCh);
  return dataStr;
}

/*!
  read an int
  
  \warning throw a vpException::ioError if the value cannot be parsed to an integer
  
  \param doc : The main xml document
  \param node : a pointer to the node to read value
  
  \return the integer value in the node
*/
int 
vpXmlParser::xmlReadIntChild (xmlDocPtr doc, xmlNodePtr node)
{
  if(node ->xmlChildrenNode == NULL){
    std::string errorMsg = "Empty node " + std::string((char*)node->name) + ", cannot read int";
    std::cerr << errorMsg << std::endl;
    throw vpException(vpException::fatalError, errorMsg);
  }
  char * val_char;
  char * control_convert;
  int val_int;

  val_char = (char *) xmlNodeListGetString(doc, node ->xmlChildrenNode, 1);
  val_int = strtol ((char *)val_char, &control_convert, 10);

  if (val_char == control_convert){
    xmlFree((xmlChar*) val_char);
    throw vpException(vpException::ioError, "cannot parse entry to int");
  }
  xmlFree((xmlChar*) val_char);

  return val_int;
}

/*!
  read an int
  
  \warning throw a vpException::ioError if the value cannot be parsed to an integer
  
  \param doc : The main xml document
  \param node : a pointer to the node to read value
  
  \return the unsigned integer value in the node
*/
unsigned int 
vpXmlParser::xmlReadUnsignedIntChild (xmlDocPtr doc, xmlNodePtr node)
{
  if(node ->xmlChildrenNode == NULL){
    std::string errorMsg = "Empty node " + std::string((char*)node->name) + ", cannot read unsigned int";
    std::cerr << errorMsg << std::endl;
    throw vpException(vpException::fatalError, errorMsg);
  }
  char * val_char;
  char * control_convert;
  unsigned int val_uint;

  val_char = (char *) xmlNodeListGetString(doc, node ->xmlChildrenNode, 1);
  val_uint = strtoul ((char *)val_char, &control_convert, 10);

  if (val_char == control_convert){
    xmlFree((xmlChar*) val_char);
    throw vpException(vpException::ioError, "cannot parse entry to int");
  }
  xmlFree((xmlChar*) val_char);

  return val_uint;
}


/*!
  read a double
  
  \warning throw a vpException::ioError if the value cannot be parsed to an integer
  
  \param doc : The main xml document
  \param node : a pointer to the node to read value
  
  \return the double value in the node
*/
double 
vpXmlParser::xmlReadDoubleChild (xmlDocPtr doc, xmlNodePtr node)
{
  if(node ->xmlChildrenNode == NULL){
    std::string errorMsg = "Empty node " + std::string((char*)node->name) + ", cannot read double";
    std::cerr << errorMsg << std::endl;
    throw vpException(vpException::fatalError, errorMsg);
  }
  char * val_char;
  char * control_convert;
  double val_double;

  val_char = (char *) xmlNodeListGetString(doc, node ->xmlChildrenNode, 1);
  val_double = strtod ((char *)val_char, &control_convert);

  if (val_char == control_convert){
    xmlFree((xmlChar*) val_char);
    throw vpException(vpException::ioError, "cannot parse entry to double");
  }
  xmlFree((xmlChar*) val_char);
  return val_double;
}

/*!
  write an array of character. 
  
  \param node : a pointer to the node to read value
  \param label : label (name of the data) of the node 
  \param value : pointer to the array of character to write
*/
void 
vpXmlParser::xmlWriteCharChild(xmlNodePtr node, const char* label, const char* value)
{
  xmlNodePtr tmp;
  tmp = xmlNewChild(node, NULL, (xmlChar*)label, (xmlChar*)value);
  xmlAddChild(node, tmp);  
}

/*!
  write an array of character.

  \param node : a pointer to the node to read value
  \param label : label (name of the data) of the node
  \param value : std::string to write;
*/
void
vpXmlParser::xmlWriteStringChild(xmlNodePtr node, const char* label, const std::string& value)
{
  xmlNodePtr tmp;
  tmp = xmlNewChild(node, NULL, (xmlChar*)label, (xmlChar*)value.c_str());
  xmlAddChild(node, tmp);
}


/*!
  write an integer. 
  
  \param node : a pointer to the node to read value
  \param label : label (name of the data) of the node 
  \param value : integer to write
*/
void 
vpXmlParser::xmlWriteIntChild(xmlNodePtr node, const char* label, const int value)
{
  char str[100];
  sprintf(str, "%d", value);
  xmlNodePtr tmp;
  tmp = xmlNewChild(node, NULL, (xmlChar*)label, (xmlChar*)str);
  xmlAddChild(node, tmp); 
}

/*!
  write an unsigned integer. 
  
  \param node : a pointer to the node to read value
  \param label : label (name of the data) of the node 
  \param value : unsigned integer to write
*/
void 
vpXmlParser::xmlWriteUnsignedIntChild(xmlNodePtr node, const char* label, const unsigned int value)
{
  char str[100];
  sprintf(str, "%u", value);
  xmlNodePtr tmp;
  tmp = xmlNewChild(node, NULL, (xmlChar*)label, (xmlChar*)str);
  xmlAddChild(node, tmp); 
}


/*!
  write a double. 
  
  \param node : a pointer to the node to read value
  \param label : label (name of the data) of the node 
  \param value : double to write
*/
void 
vpXmlParser::xmlWriteDoubleChild(xmlNodePtr node, const char* label, const double value)
{
  char str[100];
  sprintf(str, "%lf", value);
  xmlNodePtr tmp;
  tmp = xmlNewChild(node, NULL, (xmlChar*)label, (xmlChar*)str);
  xmlAddChild(node, tmp); 
}


/* -------------------------------------------------------------------------- */
/*                                MAIN METHODS                                */
/* -------------------------------------------------------------------------- */

/*!
  parse the document.
  The data in the file are stored in the attributes of the child class. This 
  method calls the readMainClass method which has to be implemented for every
  child class depending on the content to parse.
  
  \param filename : name of the file to parse
*/
void 
vpXmlParser::parse(const std::string& filename)
{
  xmlDocPtr doc;
  xmlNodePtr root_node;

  doc = xmlParseFile(filename.c_str());
  if(doc == NULL){
  	vpERROR_TRACE("cannot open file");
  	throw vpException(vpException::ioError, "cannot open file");
  }

  root_node = xmlDocGetRootElement(doc);
  if(root_node == NULL){
  	vpERROR_TRACE("cannot get root element");
  	throw vpException(vpException::ioError, "cannot get root element");
  }

  readMainClass(doc, root_node);

  xmlFreeDoc(doc);
}

/*!
  Save the content of the class in the file given in parameters.
  The data of the class are in the child class.
  This method calls the write_main_class method which has to be implemented for
  every class depending on the data to save.
  
  \param filename : the name of the file used to record the data
  \param append : if true and if the file exists, the data will be added to the
  data already in the file
*/
void 
vpXmlParser::save(const std::string& filename, const bool append)
{
  xmlDocPtr doc;
  xmlNodePtr root_node;

  doc = xmlReadFile(filename.c_str(), NULL, XML_PARSE_NOWARNING + XML_PARSE_NOERROR + XML_PARSE_NOBLANKS);
  if (doc == NULL){
    doc = xmlNewDoc ((xmlChar*)"1.0");
    root_node = xmlNewNode(NULL, (xmlChar*)main_tag.c_str());
    xmlDocSetRootElement(doc, root_node);
  }
  else{
  	if(!append){
      xmlFreeDoc(doc);
  		remove(filename.c_str());
  		doc = xmlNewDoc ((xmlChar*)"1.0");
  		root_node = xmlNewNode(NULL, (xmlChar*)main_tag.c_str());
  		xmlDocSetRootElement(doc, root_node);
  	}
  }

  root_node = xmlDocGetRootElement(doc);
  if (root_node == NULL)
  {
  	vpERROR_TRACE("problem to get the root node");
  	throw vpException(vpException::ioError, "problem to get the root node");
  }

	writeMainClass(root_node);

  xmlSaveFormatFile(filename.c_str(), doc, 1);
  xmlFreeDoc(doc);
}




#endif




