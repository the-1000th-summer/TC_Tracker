c 计算wrf绝对涡度场
c 此文件仅用于参考，不可编译运行此文件
c 数字里带D表示DOUBLE PRECISION, 0.5D0=0.5×10^0, 1.D5=1.0×10^5
c--------------------------------------------------------

c NY = south_north, NX = west_east
c NYP1 = south_north_stag, NXP1 = west_east_stag
c DUDY: 表示 du/dy, DVDX: 表示 dv/dx
c DX: ncfile@DX, 即纬向格点点距; DY: ncfile@DY, 即经向格点点距
c MSFT: 即MSFM

C NCLFORTSTART
      SUBROUTINE DCOMPUTEABSVORT(AV,U,V,MSFU,MSFV,MSFT,COR,DX,DY,NX,NY,
     +                           NZ,NXP1,NYP1)

      IMPLICIT NONE
      INTEGER NX,NY,NZ,NXP1,NYP1
      DOUBLE PRECISION U(NXP1,NY,NZ),V(NX,NYP1,NZ)
      DOUBLE PRECISION AV(NX,NY,NZ)
      DOUBLE PRECISION MSFU(NXP1,NY),MSFV(NX,NYP1),MSFT(NX,NY)
      DOUBLE PRECISION COR(NX,NY)
      DOUBLE PRECISION DX,DY
C NCLEND
      INTEGER KP1,KM1,JP1,JM1,IP1,IM1,I,J,K
      DOUBLE PRECISION DSY,DSX,DP,DUDY,DVDX,DUDP,DVDP,DTHDP,AVORT
      DOUBLE PRECISION DTHDX,DTHDY,MM

c          print*,'nx,ny,nz,nxp1,nyp1'
c          print*,nx,ny,nz,nxp1,nyp1
      DO K = 1,NZ
          DO J = 1,NY
              JP1 = MIN(J+1,NY)           !  2,3,4,5,...,NY-1,NY  ,NY
              JM1 = MAX(J-1,1)            !  1,1,2,3,...,NY-3,NY-2,NY-1
              DO I = 1,NX
                  IP1 = MIN(I+1,NX)       !  2,3,4,5,...,NX-1,NX  ,NX
                  IM1 = MAX(I-1,1)        !  1,1,2,3,...,NX-3,NX-2,NX-1
c         print *,jp1,jm1,ip1,im1
                  DSX = (IP1-IM1)*DX      ! (1,2,2,2,...,   2,   2,   1) * DX
                  DSY = (JP1-JM1)*DY      ! (1,2,2,2,...,   2,   2,   1) * DY
                  MM = MSFT(I,J)*MSFT(I,J)
c         print *,j,i,u(i,jp1,k),msfu(i,jp1),u(i,jp1,k)/msfu(i,jp1)
                  DUDY = 0.5D0* (U(I,JP1,K)/MSFU(I,JP1)+
     +                   U(I+1,JP1,K)/MSFU(I+1,JP1)-
     +                   U(I,JM1,K)/MSFU(I,JM1)-
     +                   U(I+1,JM1,K)/MSFU(I+1,JM1))/DSY*MM
                  DVDX = 0.5D0* (V(IP1,J,K)/MSFV(IP1,J)+
     +                   V(IP1,J+1,K)/MSFV(IP1,J+1)-
     +                   V(IM1,J,K)/MSFV(IM1,J)-
     +                   V(IM1,J+1,K)/MSFV(IM1,J+1))/DSX*MM
                  AVORT = DVDX - DUDY + COR(I,J)
                  AV(I,J,K) = AVORT*1.D5            ! 单位转换: s^-1 -> 10^-5 s^-1
              END DO
          END DO
      END DO
      RETURN
      END
