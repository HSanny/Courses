

/* Define hash function */
unsigned sp_hash (const struct hash_elem * elem, void * aux) 
{
    struct SP * page = hash_entry (elem, struct SP, SP_helem);
    return hash_byte (&(page->vaddr), sizeof (page->vaddr));
}

/* Define the comparison function */
unsigned sp_hash_less (const struct hash_elem *a, const struct hash_elem *b,
        void * aux) 
{
    struct SP * spa = hash_entry (a, struct SP, SP_helem);
    struct SP * spb = hash_entry (b, struct SP, SP_helem);
    return spa->uaddr < spb->uaddr;
}

bool sp_table_init (struct hash * page_table) 
{
    bool success;
    success = hash_init (page_table, sp_hash, sp_hash_less, NULL);
    return success;
}

struct SP * sp_table_put (struct hash * page_table, void * vaddr)
{
     struct SP * new = (struct *) malloc (sizeof (struct SP));

     return new;
}

struct SP * sp_table_lookup(struct hash * page_table, void * vaddr)
{
    struct SP * finding = NULL;
    if (hash_empty(page_table)) return NULL;   

    return finding;
}

struct SP * sp_table_remove(struct hash * page_table, void * vaddr)
{
    struct SP * removed;

    return removed;
}
