from fastapi import APIRouter, Depends, HTTPException, Query
from sqlalchemy.orm import Session
from typing import List, Optional
from app import schemas, models, auth, database

router = APIRouter(prefix="/photos", tags=["Photos"])

@router.post("", response_model=schemas.PhotoOut)
def create_photo(photo: schemas.PhotoCreate, db: Session = Depends(database.SessionLocal), user: models.User = Depends(auth.get_current_user)):
    new_photo = models.Photo(**photo.dict(), owner_id=user.id)
    db.add(new_photo)
    db.commit()
    db.refresh(new_photo)
    return new_photo

@router.get("", response_model=List[schemas.PhotoOut])
def list_photos(db: Session = Depends(database.SessionLocal),
                skip: int = 0, limit: int = 10,
                owner_id: Optional[int] = Query(None),
                sort: Optional[str] = Query("created_at:desc")):
    query = db.query(models.Photo)
    if owner_id:
        query = query.filter(models.Photo.owner_id == owner_id)
    if sort:
        field, order = sort.split(":")
        if field == "created_at":
            query = query.order_by(models.Photo.created_at.desc() if order == "desc" else models.Photo.created_at.asc())
    return query.offset(skip).limit(limit).all()

@router.delete("/{photo_id}")
def delete_photo(photo_id: int, db: Session = Depends(database.SessionLocal), user: models.User = Depends(auth.get_current_user)):
    photo = db.query(models.Photo).filter(models.Photo.id == photo_id).first()
    if not photo:
        raise HTTPException(status_code=404, detail="Photo not found")
    if user.id != photo.owner_id and user.role != "admin":
        raise HTTPException(status_code=403, detail="Not allowed")
    db.delete(photo)
    db.commit()
    return {"detail": "Photo deleted"}