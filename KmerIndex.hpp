#ifndef _LSONG_KMERINDEX_HEADER
#define _LSONG_KMERINDEX_HEADER

#include <stdio.h>
#include <stdint.h>
#include <map>

#include "defs.h"
#include "KmerCode.hpp"
#include "SimpleVector.hpp"

struct _indexInfo
{
	index_t idx ;
	index_t offset ;
	//int strand ;
} ;

class KmerIndex
{
private:
	std::map< uint64_t, SimpleVector<struct _indexInfo> > index ;
	SimpleVector<struct _indexInfo> nullHit ;
public:
	KmerIndex() {}
	~KmerIndex()
	{
		int sum = 0 ;
		//for ( int i = 0 ; i < hashSize ; ++i )
		//{
		//	sum += /*sizeof( hash[i] ) +*/ hash[i].Memory() ;
		//}
		//printf( "%s: %d %d %d\n", __func__, hashSize, sizeof( hash[0] ), hash[0].Memory() ) ;
	}

	void Insert( KmerCode &kmerCode, index_t poaId, index_t offset, int strand )
	{
		if ( !kmerCode.IsValid() )
			return ;
		struct _indexInfo newEntry ;
		newEntry.idx = poaId ;
		newEntry.offset = offset ;
		//newEntry.strand = strand ;
		index[ kmerCode.GetCode() ].PushBack( newEntry ) ;
		
		//printf( "%d\n", hash[key].Memory() ) ;
	}

	void Remove( KmerCode &kmerCode, index_t readId, index_t offset, int strand )
	{
	}

	SimpleVector<struct _indexInfo> *Search( KmerCode &kmerCode )
	{
		if ( !kmerCode.IsValid() )
			return &nullHit ;
		uint64_t kcode = kmerCode.GetCode() ;
		
		return &index[kcode] ;
	}

	void BuildIndexFromRead( KmerCode &kmerCode, char *s, int len, int id, int shift = 0 )
	{
		int i ;
		int kl = kmerCode.GetKmerLength() ;
		if ( len < kl )
			return ;
		kmerCode.Restart() ;
		//KmerCode rcKmerCode( kl ) ;
		KmerCode prevKmerCode ;
		for ( i = 0 ; i < kl - 1 ; ++i )
			kmerCode.Append( s[i] ) ;
		for ( ; i < len ; ++i )
		{
			kmerCode.Append( s[i] ) ;
			if ( kmerCode.IsValid() && !kmerCode.IsEqual( prevKmerCode ) )
			{
				Insert( kmerCode, id, i - kl + 1 + shift, 1 ) ;

				//rcKmerCode.SetCode( kmerCode.GetReverseComplementCode() ) ;
				//Insert( rcKmerCode, id, len - 1 - i, -1 ) ;
			}
			prevKmerCode = kmerCode ;
		}
	}
	
	// When merging or extending sequences, there kmer position will shift and change id.
	void UpdateIndex( KmerCode &kmerCode, char *s, int len, int shift, int oldId, int id ) 
	{
		int i, j ;
		int kl = kmerCode.GetKmerLength() ;
		if ( len < kl )
			return ;
		kmerCode.Restart() ;

		for ( i = 0 ; i < kl - 1 ; ++i )
			kmerCode.Append( s[i] ) ;
		for ( ; i < len ; ++i )
		{
			kmerCode.Append( s[i] ) ;
			if ( kmerCode.IsValid() )
			{
				SimpleVector<struct _indexInfo> &list = *Search( kmerCode ) ;	
				int size = list.Size() ;

				for ( j = 0 ; j < size ; ++j )
				{
					if ( list[j].idx == oldId && list[j].offset == i - kl + 1 )
					{
						list[j].idx = id ;
						list[j].offset += shift ;
						break ;
					}
				}
			}
		}

	}
} ;

#endif
