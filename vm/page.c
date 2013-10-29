

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
    return spa->vaddr < spb->vaddr;
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
    new->vaddr = vaddr;
    // TODO: assignment for more elements

    // insert into page table 
    struct hash_elem * helem = hash_insert (page_table, &new->SP_helem);
    if (helem != NULL)
        return new;
}

struct SP * sp_table_find (struct hash * page_table, void * vaddr)
{
    struct SP * finding = NULL;
    if (hash_empty(page_table)) return NULL;   

    struct SP temp;
    temp.vaddr = vaddr;

    struct hash_elem * helem = hash_find (page_table, &temp.SP_helem);
    finding = hash_entry (helem, struct SP, SP_helem);

    return finding;
}

struct SP * sp_table_remove(struct hash * page_table, void * vaddr)
{
    struct SP * removed;
    if (hash_empty(page_table)) return NULL;   

    // define the target
    struct SP temp;
    temp.vaddr = vaddr;

    // remove the element in sp table
    struct hash_elem * helem = hash_remove (page_table, &temp.SP_helem);

    if (helem != NULL)
        return removed;
}
