// ==============================
// Fichier:			TGPBooleanData.h
// Projet:			K
//
// Cr�� le:			23/2/2003
// Tabulation:		4 espaces
//
// ***** BEGIN LICENSE BLOCK *****
// Version: MPL 1.1
//
// The contents of this file are subject to the Mozilla Public License Version
// 1.1 (the "License"); you may not use this file except in compliance with
// the License. You may obtain a copy of the License at
// http://www.mozilla.org/MPL/
//
// Software distributed under the License is distributed on an "AS IS" basis,
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
// for the specific language governing rights and limitations under the
// License.
//
// The Original Code is TGPBooleanData.h.
//
// The Initial Developer of the Original Code is Paul Guyot.
// Portions created by the Initial Developer are Copyright (C) 2003-2004
// the Initial Developer. All Rights Reserved.
//
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
//
// ***** END LICENSE BLOCK *****
// ===========
// $Id: TGPBooleanData.h,v 1.4 2005/02/14 20:55:02 pguyot Exp $
// ===========

#ifndef _TGPBOOLEANDATA_H
#define _TGPBOOLEANDATA_H

#include <K/Defines/KDefinitions.h>
#include <K/GP/Data/TGPData.h>

// ISO C++
#include <string>

///
/// Classe pour des donn�es sous forme de bool�en.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test d�fini.
///
class TGPBooleanData
	:
		public TGPData
{
protected:
	///
	/// Classe de TGPBooleanData
	///
	class TClass
		:
			public TGPType
	{
	public:
		///
		/// Accesseur sur le nom de la classe de cette objet.
		///
		/// \return une cha�ne repr�sentant la classe de l'objet.
		///
		virtual TStringRef GetName( void ) const
			{
				return kName;
			}

		///
		/// D�termine si cette classe est �gale ou une sous-classe
		/// d'une classe donn�e.
		///
		/// \param inOtherClass	classe pour le test.
		/// \return \c true si cet objet est un �l�ment de la classe \c inOtherClass
		///
		virtual Boolean IsTypeOf( const TGPType& inOtherClass ) const
			{
				return &TGPBooleanData::kClass == &inOtherClass;
			}

		///
		/// R�cup�re une valeur � partir d'une cha�ne.
		///
		/// \param outValue	valeur (en sortie).
		/// \return \c true si la cha�ne a pu �tre analys�e correctement.
		///
		virtual Boolean	ValueFromString(
							TGPDataRef& outValue,
							const char* inString ) const;
	private:
		/// Nom de la classe.
		static const TStringRef	kName;
	};

public:
	///
	/// Objet qui repr�sente cette classe.
	///	
	static const TClass	kClass;	

	///
	/// Constructeur � partir d'un bool�en et d'une classe.
	///
	/// \param inBoolean	bool�en pour cette donn�e.
	/// \param inClass		classe pour cette donn�e.
	///
	TGPBooleanData( Boolean inBoolean, const TGPType& inClass = kClass );

	///
	/// R�cup�ration de la cha�ne pour ces donn�es.
	///
	/// \param inContext	contexte de l'�valuation.
	/// \return une repr�sentation sous forme de cha�ne des donn�es.
	///
	virtual const std::string&	AsString( TGPEvaluationContext* inContext ) const;

	///
	/// R�cup�ration du bool�en pour ces donn�es.
	///
	/// \param inContext	contexte de l'�valuation.
	/// \return un bool�en qui repr�sente les donn�es.
	///
	virtual Boolean				AsBoolean( TGPEvaluationContext* inContext ) const;

	///
	/// Test d'�galit�.
	///
	/// \param inAlter		donn�es avec lesquelles comparer \c this
	/// \param inContext	contexte pour la comparaison.
	/// \return \c true si \c this est �gal � \c inAlter
	///
	virtual Boolean Equal(
				const TGPData& inAlter,
				TGPEvaluationContext* inContext ) const;

	///
	/// Relation d'ordre.
	///
	/// \param inAlter		donn�es avec lesquelles comparer \c this
	/// \param inContext	contexte pour la comparaison.
	/// \return \c true si \c this est strictement sup�rieur � \c inAlter
	///
	virtual Boolean GreaterThan(
				const TGPData& inAlter,
				TGPEvaluationContext* inContext ) const;

	///
	/// �crit la donn�e sur un flux.
	///
	/// \param inStream	flux consid�r�.
	///
	virtual void	PrintToStream( std::ostream& inStream ) const;

private:
	/// \name Constantes
	static const std::string kTrueStr;	///< "true"
	static const std::string kFalseStr;	///< "false"

	/// \name Variables
	Boolean				mBoolean;
};

#endif
		// _TGPBOOLEANDATA_H

// ======================= //
// With your bare hands?!? //
// ======================= //
