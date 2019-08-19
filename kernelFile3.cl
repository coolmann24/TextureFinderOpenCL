int getTextureType(int x, int y, int z);

int compatibleRotation(int generatedType, int rotation, int blockFace);

kernel void kernelFile(global int *faces, global int *facecount, global int *found, global int *xlength, global int *zlength, global int *ymin)
{
	if(found[1] == 0){
		int foundHere=1;
		int i;
		for(i=0; i < (*facecount); i++)
		{
			int tex = getTextureType(get_global_id(0) + faces[i*5] - ((*xlength)/2), get_global_id(1) + (*ymin) + faces[i*5+1], get_global_id(2) + faces[i*5+2]- ((*zlength)/2));
			if(compatibleRotation(tex, faces[i*5 + 4], faces[i*5 + 3])==0)
			{
				foundHere=0;
				break;
			}
		}
		
		if(foundHere==1)
		{
			found[0] = get_global_id(0)- ((*xlength)/2);
			found[1] = get_global_id(1) + (*ymin);
			found[2] = get_global_id(2)- ((*zlength)/2);
			
		}
	}
}

int getTextureType(int x, int y, int z)
{
	long i = (long)(x * 3129871) ^ (long)z * (long)116129781 ^ (long)y;
	i = i * i * (long)42317861 + i * 11;
	i = i >> 16;
	
	long seed = (i ^ 0x5DEECE66D) & ((((long)1) << 48) - 1);
	seed = (seed * 0x5DEECE66D + 0xB) & ((((long)1) << 48) - 1);
	seed = (seed * 0x5DEECE66D + 0xB) & ((((long)1) << 48) - 1);
	
	int retlong = (int)(((unsigned long)seed) >> 16);
	
	if(retlong < 0) retlong*=-1;
	
	return (retlong%16);
}

int compatibleRotation(int generatedType, int rotation, int blockFace)//blockFace 0-5
{
	if(generatedType == 0)
		 {
			 if (rotation==3) return 1;
		 }
		 if(generatedType == 1)
		 {
			 if ((rotation==3 && (blockFace==0||blockFace==4))||
					 (rotation==2 && (blockFace==2))||
					 (rotation==1 && (blockFace==1||blockFace==5))||
					 (rotation==0 && (blockFace==3))) return 1;
		 }
		 if(generatedType == 2)
		 {
			 if ((rotation==3 && (blockFace==0||blockFace==1))||
					 (rotation==1 && (blockFace!=0&&blockFace!=1))) return 1;
		 }
		 if(generatedType == 3)
		 {
			 if ((rotation==3 && (blockFace==1||blockFace==4))||
					 (rotation==2 && (blockFace==3))||
					 (rotation==1 && (blockFace==0||blockFace==5))||
					 (rotation==0 && (blockFace==2))) return 1;
		 }
		 if(generatedType == 4)
		 {
			 if ((rotation==3 && (blockFace!=0&&blockFace!=1))||
					 (rotation==2 && (blockFace==1))||
					 (rotation==0 && (blockFace==0))) return 1;
		 }
		 if(generatedType == 5)
		 {
			 if ((rotation==3 && (blockFace==2))||
					 (rotation==2 && (blockFace==5))||
					 (rotation==1 && (blockFace==3))||
					 (rotation==0 && (blockFace==0||blockFace==1||blockFace==4))) return 1;
		 }
		 if(generatedType == 6)
		 {
			 if ((rotation==1 && (blockFace!=0&&blockFace!=1))||
					 (rotation==2 && (blockFace==1))||
					 (rotation==0 && (blockFace==0))) return 1;
		 }
		 if(generatedType == 7)
		 {
			 if ((rotation==3 && (blockFace==2))||
					 (rotation==2 && (blockFace==4||blockFace==0||blockFace==1))||
					 (rotation==1 && (blockFace==3))||
					 (rotation==0 && (blockFace==5))) return 1;
		 }
		 if(generatedType == 8)
		 {
			 if ((rotation==1 && (blockFace==0||blockFace==1))||
					 (rotation==3 && (blockFace!=0&&blockFace!=1))) return 1;
		 }
		 if(generatedType == 9)
		 {
			 if ((rotation==3 && (blockFace==1||blockFace==5))||
					 (rotation==2 && (blockFace==3))||
					 (rotation==1 && (blockFace==0||blockFace==4))||
					 (rotation==0 && (blockFace==2))) return 1;
		 }
		 if(generatedType == 10)
		 {
			 if (rotation==1) return 1;
		 }
		 if(generatedType == 11)
		 {
			 if ((rotation==3 && (blockFace==0||blockFace==5))||
					 (rotation==2 && (blockFace==2))||
					 (rotation==1 && (blockFace==1||blockFace==4))||
					 (rotation==0 && (blockFace==3))) return 1;
		 }
		 if(generatedType == 12)
		 {
			 if ((rotation==3 && (blockFace!=0&&blockFace!=1))||
					 (rotation==2 && (blockFace==0))||
					 (rotation==0 && (blockFace==1))) return 1;
		 }
		 if(generatedType == 13)
		 {
			 if ((rotation==3 && (blockFace==3))||
					 (rotation==2 && (blockFace==0||blockFace==1||blockFace==4))||
					 (rotation==1 && (blockFace==2))||
					 (rotation==0 && (blockFace==5))) return 1;
		 }
		 if(generatedType == 14)
		 {
			 if ((rotation==1 && (blockFace!=0&&blockFace!=1))||
					 (rotation==2 && (blockFace==0))||
					 (rotation==0 && (blockFace==1))) return 1;
		 }
		 if(generatedType == 15)
		 {
			 if ((rotation==3 && (blockFace==3))||
					 (rotation==2 && (blockFace==5))||
					 (rotation==1 && (blockFace==2))||
					 (rotation==0 && (blockFace==4||blockFace==0||blockFace==1))) return 1;
		 }
		 return 0;
}
