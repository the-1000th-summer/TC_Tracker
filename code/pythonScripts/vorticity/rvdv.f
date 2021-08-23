c 计算相对涡度场(uv2vr_cfd)
c 此文件仅用于参考，不可编译运行此文件


c MLON: x方向格点数, NLAT: y方向格点数
c GLAT: lat数组, GLON: lon数组
c RV: 相对涡度
c IOPT: 0,1,2,3

SUBROUTINE DVRFIDF(U,V,GLAT,GLON,MLON,NLAT,XMSG,IOPT,RV,IER)
IMPLICIT NONE

c relative vorticity via finite differences
c NCL: rv = uv2vr_cfd (u,v,lat,lon,cyclic)

c            xmsg = u@_FillValue
c            nlat,mlon = dimsizes (u)
c            iopt= cyclic

c relative vorticity via centered finite difference approach (rv)
c .  rv = dv/dx-du/dy+(u/a)tan(lat)   where "d" means partial derivitive
c reference: Bluestein p113-114 [was not the original reference]
c            Halt-Martin p314   [ nothing on rv] 

c assumptions:
c .   (1) latitudes  monotonically increasing  [eg: glat(2) > glat(1)]
c .                  latitudes may be unequally spaced
c .   (2) longitudes monotonically increasing  [eg: glon(2) > glon(1)]
c .                  longitudes are assmed to be equally spaced
c .   (3) if iopt=1 then the grids are cyclic in x
c .                  eg: for T42 grid mlon=128, nlat=64
c .                      cyclic point should NOT be included
c .
c                                          ! INPUT
    INTEGER MLON,NLAT
    DOUBLE PRECISION U(MLON,NLAT),V(MLON,NLAT),GLAT(NLAT),GLON(MLON),XMSG
    INTEGER IOPT

c                                          ! OUTPUT
    DOUBLE PRECISION RV(MLON,NLAT)
    INTEGER IER
C NCLEND
c                                          ! LOCAL  (dynamic/adjustable)
    DOUBLE PRECISION CLAT(NLAT),DX(NLAT),DLON,DX2(NLAT),DLON2,
    +                 DY2(NLAT),DYTOP,DYBOT,RE,RAD,RCON,
    +                 POLAT,TLATRE(NLAT)
    INTEGER ML,NL,MLSTRT,MLEND,MLM1,MLP1,JOPT

c                                          ! error chk stuff
    IER = 0
    IF (MLON.LT.1 .OR. NLAT.LT.1) THEN
        IER = 1
        RETURN
    END IF
    IF (ABS(GLAT(1)).gt.90.D0 .OR. ABS(GLAT(NLAT)).GT.90.D0) THEN
        IER = 2
        RETURN
    END IF


    RE = 6.37122D06                  ! 地球半径(米)
    RAD = 4.D0*ATAN(1.D0)/180.D0         ! 4.D0*ATAN(1.D0)为π的双精度值
    RCON = RE*RAD
    JOPT = ABS(IOPT)
c                                 ! pre-compute cos(lat)
    DO NL = 1,NLAT
        CLAT(NL)   = COS(RAD*GLAT(NL))
    END DO
c                                 ! pre-compute tan(lat)/re
c                                 ! pole pts will not be used below
    DO NL = 1,NLAT
        IF (ABS(GLAT(NL)).LT.90.D0) THEN
            TLATRE(NL) = TAN(RAD*GLAT(NL))/RE
        ELSE
            IF (GLAT(NL).EQ.90.D0) THEN
                POLAT = 0.5*(GLAT(NL)+GLAT(NL-1))
                TLATRE(NL) = TAN(RAD*POLAT)/RE
            ELSE
                POLAT = 0.5*(GLAT(NL)+GLAT(NL+1))
                TLATRE(NL) = TAN(RAD*POLAT)/RE
            END IF
        END IF
    END DO
    
c                                          ! initialize to msg
    DO NL = 1,NLAT
        DO ML = 1,MLON
            RV(ML,NL) = XMSG
        END DO
    END DO
c                                          ! calculate "1/dy" [bot, top]
    DYBOT = 1.D0/ (RCON* (GLAT(2)-GLAT(1)))
    DYTOP = 1.D0/ (RCON* (GLAT(NLAT)-GLAT(NLAT-1)))
c                                          ! calculate "1/(2*dy)"
    DO NL = 2,NLAT - 1
        DY2(NL) = 1.D0/ (RCON* (GLAT(NL+1)-GLAT(NL-1)))
    END DO
c                                       ! calculate "1/dx" [left, right]
c                                       ! and "1/(2*dx)
    DLON = GLON(2) - GLON(1)
    DLON2 = GLON(3) - GLON(1)

    DO NL = 1,NLAT
        IF (ABS(GLAT(NL)).NE.90.D0) THEN             ! 不在极点
            DX(NL) = 1.D0/ (RCON*DLON*CLAT(NL))
            DX2(NL) = 1.D0/ (RCON*DLON2*CLAT(NL))
        ELSE                                         ! 在极点
            DX(NL) = 0.0D0
            DX2(NL) = 0.0D0
        END IF
    END DO
c                                          ! set subscript range
    IF (JOPT.EQ.1 .OR. JOPT.EQ.3) THEN
        MLSTRT = 1
        MLEND = MLON
    ELSE
        MLSTRT = 2
        MLEND = MLON - 1
    END IF
c                                          ! longitude loop
    DO ML = MLSTRT,MLEND
c                                          ! iopt=0 or 1
        MLM1 = ML - 1
        MLP1 = ML + 1
        IF (ML.EQ.1) MLM1 = MLON
        IF (ML.EQ.MLON) MLP1 = 1
c                                          ! rv in grid body
        DO NL = 2,NLAT - 1
            IF (V(MLP1,NL).NE.XMSG .AND. V(MLM1,NL).NE.XMSG .AND.
    +            U(ML,NL+1).NE.XMSG .AND. U(ML,NL-1).NE.XMSG .AND.
    +            U(ML,NL)  .NE.XMSG) THEN

                RV(ML,NL) = (V(MLP1,NL)-V(MLM1,NL))*DX2(NL) -
    +                        (U(ML,NL+1)-U(ML,NL-1))*DY2(NL) +
    +                         U(ML,NL)*TLATRE(NL) 

            END IF
        END DO
C                                ! 上下边界不是missing value
        IF (JOPT.GE.2) THEN 
c                                   ! bottom bound (nl=1)
            IF (V(MLP1,1).NE.XMSG .AND. V(MLM1,1).NE.XMSG .AND.
    +            U(ML,2)  .NE.XMSG .AND. U(ML,1)  .NE.XMSG) THEN

                RV(ML,1) = (V(MLP1,1)-V(MLM1,1))*DX2(1) -
    +                       (U(ML,2)-U(ML,1))*DYBOT      +
    +                        U(ML,1)*TLATRE(1)
            END IF
c                                   ! top bound   (nl=nlat)
            IF (V(MLP1,NLAT).NE.XMSG .AND. V(MLM1,NLAT).NE.XMSG .AND.
    +            U(ML,NLAT)  .NE.XMSG .AND. U(ML,NLAT-1).NE.XMSG)THEN

                RV(ML,NLAT) = (V(MLP1,NLAT)-V(MLM1,NLAT))*DX2(NLAT) -
    +                          (U(ML,NLAT)-U(ML,NLAT-1))*DYTOP       +
    +                           U(ML,NLAT)*TLATRE(NLAT)
            END IF
        END IF
    END DO
c                                   ! ?left/right bound? 计算左右边界相对涡度值
    IF (JOPT.EQ.2) THEN
        DO NL = 2,NLAT - 1
c                                   ! left bound (ml=1)
            IF (V(2,NL).NE.XMSG   .AND. V(1,NL)  .NE.XMSG .AND.
    +            U(1,NL+1).NE.XMSG .AND. U(1,NL-1).NE.XMSG .AND.
    +            U(1,NL)  .NE.XMSG) THEN

                RV(1,NL) = (V(2,NL)-V(1,NL))*DX(NL)      -
    +                       (U(1,NL+1)-U(1,NL-1))*DY2(NL) +
    +                        U(1,NL)*TLATRE(NL)
            END IF
c                                   ! right bound (ml=mlon)
            IF (V(MLON,NL)  .NE.XMSG .AND. V(MLON-1,NL).NE.XMSG .AND.
    +            U(MLON,NL+1).NE.XMSG .AND. U(MLON,NL-1).NE.XMSG .AND.
    +            U(MLON,NL)  .NE.XMSG) THEN

                RV(MLON,NL) = (V(MLON,NL)-V(MLON-1,NL))*DX(NL)    -
    +                          (U(MLON,NL+1)-U(MLON,NL-1))*DY2(NL) +
    +                           U(MLON,NL)*TLATRE(NL)
            END IF
        END DO
    END IF
c                                   ! special at +/-90 use average
    DO NL = 1,NLAT,NLAT - 1
    IF (ABS(GLAT(NL)).EQ.90.D0) CALL DUSEAVE(RV(1,NL),MLON,XMSG)
    END DO
c                                   ! special for corners (jopt=2 only)
c                                   ! use linear extrapolation from
c                                   ! two directions
    IF (JOPT.EQ.2) THEN
        CALL DLNEXTRP(RV,MLON,NLAT,XMSG)
    END IF

    RETURN
END

c X: RV
c 此子程序处理四角的相对涡度(boundOpt为2的情况下一定会执行)

SUBROUTINE DLNEXTRP(X,MLON,NLAT,XMSG)
    IMPLICIT NONE

c linearly extrapolate from two directions [take average]
c .   for the corner pts

    INTEGER MLON,NLAT
    DOUBLE PRECISION X(MLON,NLAT),XMSG

    INTEGER ML,NL

    DO NL = 1,NLAT,NLAT - 1
        DO ML = 1,MLON,MLON - 1
            IF (X(ML,NL).EQ.XMSG) THEN
c                                            ! 左下角的点
                IF (NL.EQ.1 .AND. ML.EQ.1) THEN
                    IF (X(ML,NL+1).NE.XMSG .AND.
    +                    X(ML,NL+2).NE.XMSG .AND.
    +                    X(ML+1,NL).NE.XMSG .AND.
    +                    X(ML+2,NL).NE.XMSG) THEN
                        X(ML,NL) = (2.D0*X(ML,NL+1)-X(ML,NL+2)+
    +                               2.D0*X(ML+1,NL)-X(ML+2,NL))*0.5D0
                    END IF
                END IF
c                                            ! 右下角的点
                IF (NL.EQ.1 .AND. ML.EQ.MLON) THEN
                    IF (X(ML,NL+1).NE.XMSG .AND.
    +                    X(ML,NL+2).NE.XMSG .AND.
    +                    X(ML-1,NL).NE.XMSG .AND.
    +                    X(ML-2,NL).NE.XMSG) THEN
                        X(ML,NL) = (2.D0*X(ML,NL+1)-X(ML,NL+2)+
    +                               2.D0*X(ML-1,NL)-X(ML-2,NL))*0.5D0
                    END IF
                END IF
c                                            ! 左上角的点
                IF (NL.EQ.NLAT .AND. ML.EQ.1) THEN
                    IF (X(ML,NL-1).NE.XMSG .AND.
    +                    X(ML,NL-2).NE.XMSG .AND.
    +                    X(ML+1,NL).NE.XMSG .AND.
    +                    X(ML+2,NL).NE.XMSG) THEN
                        X(ML,NL) = (2.D0*X(ML,NL-1)-X(ML,NL-2)+
    +                               2.D0*X(ML+1,NL)-X(ML+2,NL))*0.5D0
                    END IF
                END IF
c                                            ! 右上角的点
                IF (NL.EQ.NLAT .AND. ML.EQ.MLON) THEN
                    IF (X(ML,NL-1).NE.XMSG .AND.
    +                    X(ML,NL-2).NE.XMSG .AND.
    +                    X(ML-1,NL).NE.XMSG .AND.
    +                    X(ML-2,NL).NE.XMSG) THEN
                        X(ML,NL) = (2.D0*X(ML,NL-1)-X(ML,NL-2)+
    +                               2.D0*X(ML-1,NL)-X(ML-2,NL))*0.5D0
                    END IF
                END IF
            END IF
        END DO
    END DO

    RETURN
END

SUBROUTINE DUSEAVE(X,MLON,XMSG)
    IMPLICIT NONE
c calculate average and use at all input grid points: +/- 90 only
    INTEGER MLON
    DOUBLE PRECISION X(MLON),XMSG
c                                   ! local
    INTEGER ML
    DOUBLE PRECISION AVE,CNT

    AVE = 0.0D0
    CNT = 0.0D0
    DO ML = 1,MLON
        IF (X(ML).NE.XMSG) THEN
            AVE = AVE + X(ML)
            CNT = CNT + 1.0D0
        END IF
    END DO

    IF (CNT.NE.0.0D0) THEN
        AVE = AVE/CNT
        DO ML = 1,MLON
            X(ML) = AVE
        END DO
    END IF

    RETURN
END
    